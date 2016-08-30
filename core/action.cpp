#include <core/action.h>
#include <service/controller_manager.h>
#include <core/request.h>
#include <core/context.h>
#include <tools/property_set.h>
#include <core/session.h>
#include <core/put_definition.h>
#include <core/state_machine.h>
#include <tools/type_checker.h>
#include <core/result.h>
#include <core/workflow.h>
#include <iomanip>
#include <iostream>


Action::Action(const std::string & name):
    Logged("wkf.action"),
    Jsonable(),
    action_id(0), name(name),
    propertyset(new PropertySet()) {
    propertyset->setGuarded(true);
    
}

Action::~Action() {
}

bool Action::checkInputs(SessionPtr session, ErrorReport & er) const {
    for(const auto & id: getInputs()) {
        auto ctx = session->getInput(getActionId(), id.put_name);
        
        if(not ctx) {
            // No context was provided, is it allowed ?
            if(id.mandatory) {
                std::stringstream str;
                str << fingerprint(session) << " No context provided for input " << id.put_name;
                
                BOOST_LOG_SEV(logger,Trace) << fingerprint(session) << " No context provided for input: " << id.put_name << " even if mandatory";
                er.setError("action.input.nil", str.str());
                return false;
            }
        } else {
            // A context was provided. Does it match requirements.
            if(id.checker->check(ctx)) {
                // it matches requirement.
                if(id.ignoreEmpty) {
                    auto lctx = boost::dynamic_pointer_cast<ListCtx>(ctx);
                    if(lctx and lctx->isEmpty()) {
                        // this will refuse execution
                        // as the provided list is empty.
                        return false;
                    }
                }
                
                
            } else {
                
                BOOST_LOG_SEV(logger,Trace) << fingerprint(session) << " Invalid context provided for input: " << id.put_name << " (provided: " << ctx->getType() << ", expected: " << id.checker->generate()->getType() <<")";
                
                std::stringstream str;
                str << fingerprint(session) << " Invalid context provided for input " << id.put_name << " (provided: " << ctx->getType() << ", expected: " << id.checker->generate()->getType() <<")";
                er.setError("action.input.invalid", str.str());
                return false;
            }
            
        }
    }
    return true;
}

bool Action::checkOutputs(SessionPtr session, ErrorReport & er) const {
    for(const auto & id: getOutputs()) {
        auto ctx = session->getOutput(getActionId(), id.put_name);
        
        if(not ctx) {
            // No context was provided, is it allowed ?
            if(id.mandatory) {
                std::stringstream str;
                str << fingerprint(session) << " No context provided for output " << id.put_name;
                er.setError("action.output.nil", str.str());
                return false;
            }
        } else {
            // A context was provided. Does it match requirements.
            if(id.checker->check(ctx)) {
                // it matches requirement.
                return true;
            } else {
                // test for skip !
                if(boost::dynamic_pointer_cast<SkipCtx>(ctx) and id.allowSkip) {
                    // ahah ! that's a skip
                    continue;
                }
                
                std::stringstream str;
                str << fingerprint(session) << " Invalid context provided for output " << id.put_name << " (provided: " << ctx->getType() << ", expected: " << id.checker->generate()->getType() <<")";
                er.setError("action.output.invalid", str.str());
                return false;
            }
            
        }
    }
    return true;
}

Result Action::perform(SessionPtr session) const {
    
    BOOST_LOG_SEV(logger, Error) << "Can't execute this action as Perform function wasn't implemented.";
    return error(session, "action.perform.unimplemented","Can't execute this action as Perform function wasn't implemented.");
}

Result Action::replyReceived(SessionPtr session, RequestPtr) const {
    BOOST_LOG_SEV(logger, Error) << "Didn't expect a reply to this action.";
    return error(session, "action.reply.unimplemented","Didn't expect a reply to this action.");
}

bool Action::canPerform(SessionPtr , ErrorReport & ) const {
    return true;
}

const std::string & Action::getName() const{
    return name;
}

void Action::setName(const std::string & n) {
    name = n;
}

int32_t Action::getActionId() const {
    return action_id;
}

void Action::setActionId(int32_t aid) {
    action_id = aid;
}

PropertySetPtr Action::properties() {
    return propertyset;
}

StateMachinePtr Action::getStateMachine() const {
    return state_machine;
}

void Action::setStateMachine(StateMachinePtr sm) {
    state_machine = sm;
}

Result Action::done() const {
    Result r;
    r.action_id = getActionId();
    r.type = EType::Done;
    return r;
}

Result Action::wait() const {
    Result r;
    r.action_id = getActionId();
    r.type = EType::Wait;
    return r;
}

Result Action::async() const {
    Result r;
    r.action_id = getActionId();
    r.type = EType::Async;
    return r;
}

Result Action::error(SessionPtr session, const std::string & err_key, const std::string & error_message) const {
    Result r;
    r.action_id = getActionId();
    r.type = EType::Error;
    r.error.reset(new ErrorReport(session->getOriginalRequest()->getTarget(), err_key, error_message));
    return r;
}

void Action::asyncDone(SessionPtr session) const {
    Result r;
    r.action_id = getActionId();
    r.type = EType::Done;
    
    getStateMachine()->actionAsyncFinished(session, r);
}

void Action::asyncWait(SessionPtr session) const {
    Result r;
    r.action_id = getActionId();
    r.type = EType::Wait;
    
    getStateMachine()->actionAsyncFinished(session, r);
}

void Action::asyncError(SessionPtr session, const std::string & err_key, const std::string & error_message) const {
    Result r;
    r.action_id = getActionId();
    r.type = EType::Error;
    r.error.reset(new ErrorReport(session->getOriginalRequest()->getTarget(), err_key, error_message));

    getStateMachine()->actionAsyncFinished(session, r);
}


std::string Action::fingerprint(SessionPtr session) const {
    std::stringstream str;
    str << "[" << getStateMachine()->getWorkflow()->getName()
       << "][" << getName()
       << "][" << session->getOriginalRequest()->shortRequestId()
       << "][" << shortId(session->getOriginalRequest()->getId()) << "]";
    return str.str();
}

double Action::doubleValue(SessionPtr session, const std::string & key, double def) const {
    // well def config is Session(byAction) > Session > Action
    
    if(session->getBypass(getActionId())->hasProperty(key))
        return session->getBypass(getActionId())->getDoubleProperty(key, def);
    if(session->getBypass()->hasProperty(key))
       return session->getBypass()->getDoubleProperty(key,def);
    if(propertyset->hasProperty(key))
       return propertyset->getDoubleProperty(key, def);
       
    return def;
}

bool Action::boolValue(SessionPtr session, const std::string & key, bool def) const {
    
    if(session->getBypass(getActionId())->hasProperty(key))
        return session->getBypass(getActionId())->getBoolProperty(key, def);
    if(session->getBypass()->hasProperty(key))
       return session->getBypass()->getBoolProperty(key,def);
       if(propertyset->hasProperty(key))
       return propertyset->getBoolProperty(key, def);
       
       return def;
}

std::string Action::stringValue(SessionPtr session, const std::string & key, const std::string & def ) const {
    
    if(session->getBypass(getActionId())->hasProperty(key))
        return session->getBypass(getActionId())->getStringProperty(key, def);
    if(session->getBypass()->hasProperty(key))
       return session->getBypass()->getStringProperty(key,def);
       if(propertyset->hasProperty(key))
       return propertyset->getStringProperty(key, def);
       
       return def;
}

uint32_t Action::uintValue(SessionPtr session, const std::string & key, uint32_t def) const {
    
    if(session->getBypass(getActionId())->hasProperty(key))
        return session->getBypass(getActionId())->getUintProperty(key, def);
    if(session->getBypass()->hasProperty(key))
       return session->getBypass()->getUintProperty(key,def);
       if(propertyset->hasProperty(key))
       return propertyset->getUintProperty(key, def);
       
       return def;
}

ContextPtr Action::customValue(SessionPtr session, const std::string & key, ContextPtr def) const {
    
    if(session->getBypass(getActionId())->hasProperty(key))
        return session->getBypass(getActionId())->getCustomProperty(key, def);
    if(session->getBypass()->hasProperty(key))
        return session->getBypass()->getCustomProperty(key,def);
    if(propertyset->hasProperty(key))
        return propertyset->getCustomProperty(key, def);
    
    return def;
}

void Action::defineInput(const std::string & name, TypeChecker* checker , bool mandatory) {
    PutDefinition d;
    d.put_name = name;
    d.checker.reset(checker);
    d.mandatory = mandatory;
    
    defineInput(d);
}

void Action::defineInput(const PutDefinition & d) {
    //! @todo check that there aren't twice the same put name defined.
    
    inputs.insert(d);
}

void Action::defineOutput(const std::string & name, TypeChecker * checker, bool mandatory) {
    PutDefinition d;
    d.put_name = name;
    d.checker.reset(checker);
    d.mandatory = mandatory;

    defineOutput(d);
}

void Action::defineOutput(const PutDefinition & d) {
    //! @todo check that there aren't twice the same put name defined.
    outputs.insert(d);
}

ContextPtr Action::getInput(SessionPtr session, const std::string & name) {
    return session->getInput(getActionId(), name);
}

void Action::setOutput(SessionPtr session, const std::string & name, ContextPtr ctx) {
    session->setOutput(getActionId(), name, ctx);
}


const std::set<PutDefinition> & Action::getInputs() const {
    return inputs;
}

const std::set<PutDefinition> & Action::getOutputs() const {
    return outputs;
}

Result Action::executeSyncRequest(SessionPtr session, RequestPtr req) {
    req = prepareSyncRequest(session, req);
    ControllerManager::getInstance()->perform(req);
    return wait();
}

RequestPtr Action::prepareSyncRequest(SessionPtr session, RequestPtr req) {
    if(not req)
        req.reset(new Request());
    req->setReply(session->getOriginalRequest()->getTarget());
    req->getReply().execution_level = session->getCurrentExecutionLevel();
    req->getReply().action = getActionId();
    req->getReply().target = ETargetAction::Reply;
    req->setRequestId(session->getOriginalRequest()->getRequestId());
    return req;
}

void Action::executeAsyncRequest(RequestPtr req) {
    ControllerManager::getInstance()->perform(req);
}

bool Action::canHandleError(SessionPtr) const {
    return false;
}

std::string Action::actionLog() const {
    
    std::stringstream str;
    str << "[" << std::setw(2) << std::setfill(' ');
    switch(getActionId()) {
        case (int32_t)Step::Cleanup:
            str << "C";
            break;
        case (int32_t)Step::Die:
            str << "D";
            break;
        case (int32_t)Step::Error:
            str << "E";
            break;
        case (int32_t)Step::Finish:
            str << "F";
            break;
        case (int32_t)Step::Interrupt:
            str << "I";
            break;
        case (int32_t)Step::Status:
            str << "S";
            break;
        default:
            str << getActionId();
    };
    str <<" " << getName() << "]";
    return str.str();
}

void Action::save(boost::property_tree::ptree & root) const {
    
}

void Action::load(const boost::property_tree::ptree & root) {
    
}



OSTREAM_HELPER_IMPL(Action, obj) {
    out << "[Action] name: " << obj.getName() << ", id: " << obj.getActionId() << " ";
    return out;
}

DefaultNextAction::DefaultNextAction() : Action("Next") {
    
}

DefaultNextAction::~DefaultNextAction() {
    
}

Result DefaultNextAction::perform(SessionPtr) const {
    return done();
}
