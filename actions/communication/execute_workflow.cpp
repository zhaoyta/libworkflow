#include <actions/communication/execute_workflow.h>
#include <service/controller_manager.h>
#include <core/controller.h>
#include <tools/error_report.h>
#include <core/state_machine.h>
#include <core/bindings.h>
#include <core/put_definition.h>
#include <tools/property.h>
#include <tools/property_set.h>


ExecuteWorkflow::ExecuteWorkflow() : Action("ExecuteWorkflow") {
    properties()->defineBoolProperty("dynamic", true, "If set to true, will refresh inputs/outputs expectations and property transfert upon checkInputs.");
    properties()->defineStringProperty("controller", "default", "Targeted controller");
    properties()->defineStringProperty("workflow", "", "Targeted Workflow");
    PropertySetPtr ps(new PropertySet());
    properties()->defineCustomProperty("customization", ps, "Will be provided as general customization to the request");
    properties()->defineBoolProperty("sync", true, " Tell whether the call should be synchroneous, or not.");
    properties()->defineBoolProperty("allow_error", false, "When set to true, if targeted sync workflow fails, this one won't fail.");
    prepared = false;
}

ExecuteWorkflow::ExecuteWorkflow(const std::string & controller, const std::string & workflow) : Action("ExecuteWorkflow") {
    properties()->defineBoolProperty("dynamic", false, "If set to true, will refresh inputs/outputs expectations and property transfert upon checkInputs.");
    properties()->defineStringProperty("controller", controller, "Targeted controller");
    properties()->defineStringProperty("workflow", workflow, "Targeted workflow");
    PropertySetPtr ps(new PropertySet());
    properties()->defineCustomProperty("customization", ps, "Will be provided as general customization to the request");
    properties()->defineBoolProperty("sync", true, " Tell whether the call should be synchroneous, or not.");
    properties()->defineBoolProperty("allow_error", false, "When set to true, if targeted sync workflow fails, this one won't fail.");
    
    prepared = false;
    
}

ExecuteWorkflow::~ExecuteWorkflow() {
    
}

Result ExecuteWorkflow::perform(SessionPtr session) const {
    if(boolProperty(session, "sync")) {
        auto req = prepareSyncRequest(session);
        req->getTarget().controller = stringProperty(session, "controller");
        req->getTarget().workflow = stringProperty(session, "workflow");
        req->setBypass(customCastedProperty<PropertySet>(session, "customization"));
        return executeSyncRequest(session, req);
    } else {
        RequestPtr req(new Request());
        req->getTarget().controller = stringProperty(session, "controller");
        req->getTarget().workflow = stringProperty(session, "workflow");
        req->getReply().target = ETargetAction::NoReply; // we're async here.
        req->setBypass(customCastedProperty<PropertySet>(session, "customization"));
        
        return done();
    }
}

Result ExecuteWorkflow::replyReceived(SessionPtr session, RequestPtr reply) const {
    if(boolProperty(session, "sync")) {
        BOOST_LOG_SEV(logger, Error) << fingerprint(session) << " Received and un expected reply from: " << reply->getReply();
    } else {
        // Do the thing. (map provided contexts to outputs.)
        if(reply->getErrorReport()->isSet() and boolProperty(session, "allow_error")) {
            BOOST_LOG_SEV(logger, Info) << fingerprint(session) << " replyReceived an Error, but it has been allowed... thus forwarding default constructed outputs...";
            for(auto kv: getOutputs()) {
                setOutput(session, kv.first, kv.second.checker->generate());
            }
        } else if( not boolProperty(session, "allow_error")) {
            BOOST_LOG_SEV(logger, Error) << fingerprint(session) << "replyReceived an unexpected error. It wasn't caught by state machine, albeit our unability to deal with it. Forwarding the error";
            ErrorReportPtr ner(new ErrorReport(reply->getTarget(), reply->getErrorReport()));
            return error(session, ner);
        } else {
            for(auto kv:reply->getContext()->getContexts())
                setOutput(session, kv.first, kv.second);
        }
    }
    
    return done();
}

bool ExecuteWorkflow::canHandleError(SessionPtr session) const {
    return boolProperty(session, "allow_error");
}

bool ExecuteWorkflow::checkInputs(SessionPtr session, ErrorReport & er) const {
    if(boolProperty(session, "dynamic") or not prepared){
        bool res = const_cast<ExecuteWorkflow*>(this)->prepare(session);
        if(not res) {
            
            er.setError("execute.unknown.workflow", "Can't execute this action, as requested workflow to execute is unknown");
            return false;
        }
    }
    
    return Action::checkInputs(session,er);
}

bool ExecuteWorkflow::prepare(SessionPtr session) {
    // check if we're still prepared or not.
    auto controller = stringProperty(session, "controller");
    auto workflow = stringProperty(session, "workflow");
    
    if(prepared and session and boolProperty(session, "dynamic")) {
        prepared = session->getBypass(getActionId())->getStringProperty("__local_workflow") == workflow and
        session->getBypass(getActionId())->getStringProperty("__local_controller") == controller;
    }
    if(prepared)
        return true;
    
    
    BOOST_LOG_SEV(logger, Info) << fingerprint(session) << " is getting configured to work with: " << controller << ": " << workflow ;
    
    auto ctrl = ControllerManager::getInstance()->getController(controller);
    if(not ctrl) {
        BOOST_LOG_SEV(logger, Error) << fingerprint(session) << " Unknown controller: " << controller << " requested ...";
        return false;
    }
    auto wkf = ctrl->getWorkflow(workflow);
    if(not wkf) {
        BOOST_LOG_SEV(logger, Error) << fingerprint(session) << " Unknown workflow: " << workflow << " within controller: " << controller << " requested ...";
        return false;
    }
    
    auto sm = wkf->getStateMachine();
    
    clearInputs();
    clearOutputs();
    
    auto actions = sm->getActions();
    auto starters = sm->getStarters();
    auto enders = sm->getEnders();
    auto exposed = sm->getExposedProperties();
    
    
    for(const auto & start: starters) {
        if(getInputs().count(start.getWorkflowInput()) == 0) {
            auto act = actions.at(start.getActionId());
            if(not start.getActionInput().empty()) {
                auto input = act->getInputs().at(start.getActionInput());
                defineInput(input);
            }
            
        } else {
            // we need to ensure we've got the strictess of inputs.
            auto act = actions.at(start.getActionId());
            auto input = act->getInputs().at(start.getActionInput());
            auto & our_input = getInputs()[start.getActionInput()];
            if(input.mandatory)
                our_input.mandatory = true;
            if(input.allowSkip)
                our_input.allowSkip = true;
            if(input.ignoreEmpty)
                our_input.ignoreEmpty = true;
        }
    }
    
    
    for(const auto & end: enders) {
        if(getOutputs().count(end.getFromActionOutput()) == 0) {
            auto act = actions.at(end.getFromActionId());
            auto output = act->getOutputs().at(end.getFromActionOutput());
            defineOutput(output);
        } else {
            // we need to ensure we've got the strictess of inputs.
            auto act = actions.at(end.getFromActionId());
            auto output = act->getOutputs().at(end.getFromActionOutput());
            auto & our_output = getOutputs()[end.getFromActionOutput()];
            if(output.mandatory)
                our_output.mandatory = true;
            if(output.allowSkip)
                our_output.allowSkip = true;
            if(output.ignoreEmpty)
                our_output.ignoreEmpty = true;
        }
    }
    
    // State machine should hold what is exposed or not. ask it for a nice list and set it here.
    
    for(const auto & kv : exposed) {
        auto aid = kv.first;
        auto prop = actions[aid]->properties();
        for(const auto & ka: kv.second) {
            auto property = ka.second;
            if(prop->getBoolProperties().count(property)> 0) {
                properties()->setBoolProperty(prop->getBoolSProperty(property));
                continue;
            }
            
            if(prop->getDoubleProperties().count(property)> 0) {
                properties()->setDoubleProperty(prop->getDoubleSProperty(property));
                continue;
            }
            
            if(prop->getStringProperties().count(property)> 0) {
                properties()->setStringProperty(prop->getStringSProperty(property));
                continue;
            }
            
            if(prop->getUintProperties().count(property)> 0) {
                properties()->setUintProperty(prop->getUintSProperty(property));
                continue;
            }
            
            if(prop->getCustomProperties().count(property)> 0) {
                properties()->setCustomProperty(prop->getCustomSProperty(property));
                continue;
            }
        }
    }
    
    if(session) {
        session->getBypass(getActionId())->setStringProperty("__local_workflow",workflow);
        session->getBypass(getActionId())->setStringProperty("__local_controller",controller);
    }
    prepared = true;
    return true;
}


ErrorReport ExecuteWorkflow::expectInput(const std::string & key) const {
    ErrorReport er;
    if(not prepared){
        bool res = const_cast<ExecuteWorkflow*>(this)->prepare(SessionPtr());
        if(not res) {
            er.setError("execute.unknown.workflow", "Can't execute this action, as requested workflow to execute is unknown");
            return er;
        }
    }
    
    if(boolProperty(SessionPtr(),"dynamic")) {
        return er;
    }
    
    return Action::expectInput(key);
}

ErrorReport ExecuteWorkflow::expectOutput(const std::string & key) const {
    ErrorReport er;
    if(not prepared){
        bool res = const_cast<ExecuteWorkflow*>(this)->prepare(SessionPtr());
        if(not res) {
            er.setError("execute.unknown.workflow", "Can't execute this action, as requested workflow to execute is unknown");
            return er;
        }
    }
    
    if(boolProperty(SessionPtr(),"dynamic")) {
        return er;
    }
    
    return Action::expectOutput(key);
}

