#include <core/state_machine.h>
#include <core/action_wrappers.h>
#include <core/request.h>
#include <core/session.h>
#include <core/action.h>
#include <core/context.h>
#include <core/result.h>
#include <core/bindings.h>
#include <core/workflow.h>
#include <core/steps.h>
#include <core/action.h>
#include <core/put_definition.h>
#include <tools/action_factory.h>
#include <tools/property.h>
#include <core/controller.h>


StateMachine::StateMachine() : Jsonable(), Logged("wkf.sm"), boost::enable_shared_from_this<StateMachine>() {
        
        // define basic workflow.
    
}

StateMachine::~StateMachine() {
    actions.clear();
}

void StateMachine::init() {
    addAction(Step::Initialisation, new Action("Action"), {
        OutputBinding(Step::Initialisation, "", Step::Finish, "")
    });
    addAction(Step::Error, new DefaultNextAction(), {
        OutputBinding(Step::Error, "", Step::Cleanup, "")
    });
    addAction(Step::Finish, new DefaultNextAction(), {
        OutputBinding(Step::Finish, "", Step::Cleanup, "")
    });
    addAction(Step::Cleanup, new DefaultNextAction(), {
        OutputBinding(Step::Cleanup, "", Step::Die, "")
    });
}

WorkflowWPtr StateMachine::getWorkflow() {
    return workflow;
}

void StateMachine::setWorkflow(WorkflowWPtr wk) {
    workflow_name = wk.lock()->getName();
    workflow = wk;
}

std::vector<std::string> StateMachine::getExpectedOutput() const {
    std::vector<std::string> res;
    for(const auto & kv: outputs) {
        for(const auto & output: kv.second) {
            if(output.getToActionId() == (int32_t)Step::Finish) {
                res.push_back((output.getToActionInput()));
            }
        }
    }
    
    return res;
}

void StateMachine::addAction(Step action_id, Action* action, const std::vector<OutputBinding> & bindings) {
    addAction((int32_t)action_id, ActionPtr(action), bindings);
}


void StateMachine::addAction(Step action_id, ActionPtr action, const std::vector<OutputBinding> & bindings) {
    addAction((int32_t)action_id, action, bindings);
}

void StateMachine::addAction(int32_t action_id, Action* action, const std::vector<OutputBinding> & bindings) {
    addAction(action_id, ActionPtr(action), bindings);
}

void StateMachine::addAction(int32_t action_id, ActionPtr action, const std::vector<OutputBinding> & bindings) {
    
    
    // add wrapper.
    if(action_id == (int32_t)Step::Interrupt) {
        action = ActionWrapperPtr(new InterruptWrapper(action));
    } else if(action_id == (int32_t)Step::Error) {
        action = ActionWrapperPtr(new ErrorWrapper(action));
        
    } else if(action_id == (int32_t)Step::Finish) {
        action = ActionWrapperPtr(new FinishWrapper(action));
        
    } else if(action_id == (int32_t)Step::Cleanup) {
        action = ActionWrapperPtr(new CleanupWrapper(action));
    } else if(action_id < 0) {
        // reject negative id without reason.
        BOOST_LOG_SEV(logger, Error) << workflow_name << action->logAction() << " Failed to be added, due to invalid action_id on setup " << action_id;
        return;
    }
    
    action->setStateMachine(shared_from_this());
    action->setActionId(action_id);
    
    actions[action_id] = action;
    outputs[action_id].clear();
    for(OutputBinding ob: bindings) {
        ob.setFromActionId(action_id);
        outputs[action_id].push_back(ob);
    }
    
    
    // ensure that we know what actions are necessary for execution of another one.
    // entirely rebuild input map structure (as one may replace an action by another.
    // furthermore this is one time job.
    inputs_map.clear();
    for(const auto & kv: outputs) {
        for(auto output: kv.second) {
            inputs_map[output.getToActionId()].insert(output.getFromActionId());
        }
    }
    
    // seek all exposed properties of this action ... and register it.
    auto props = action->properties();
    for(const auto & kv: props->getBoolProperties()) {
        if(kv.second.exposed) {
            expose(action_id, kv.first);
        }
    }
    for(const auto & kv: props->getDoubleProperties()) {
        if(kv.second.exposed) {
            expose(action_id, kv.first);
        }
    }
    for(const auto & kv: props->getUintProperties()) {
        if(kv.second.exposed) {
            expose(action_id, kv.first);
        }
    }
    for(const auto & kv: props->getStringProperties()) {
        if(kv.second.exposed) {
            expose(action_id, kv.first);
        }
    }
    for(const auto & kv: props->getCustomProperties()) {
        if(kv.second.exposed) {
            expose(action_id, kv.first);
        }
    }
}

void StateMachine::addInput(const InputBinding & input) {
    starters.push_back(input);
}

void StateMachine::unexpose(int32_t action) {
    exposed_properties.erase(action);
}

void StateMachine::unexpose(int32_t action, const std::string & alias) {
    exposed_properties[action].erase(alias);
}

void StateMachine::expose(int32_t action, const std::string & key, const std::string & alias) {
    exposed_properties[action][alias] = key;
}

void StateMachine::expose(int32_t action, const std::string & key) {
    exposed_properties[action][key] = key;
}

const std::map<int32_t, std::map<std::string, std::string> > & StateMachine::getExposedProperties() const {
    return exposed_properties;
}


const std::map<int32_t, ActionPtr> & StateMachine::getActions() const {
    return actions;
}

const std::vector<InputBinding> & StateMachine::getStarters() const {
    return starters;
}

std::vector<OutputBinding> StateMachine::getEnders() const {
    std::vector<OutputBinding> res;
    for(const auto & kv: outputs) {
        for(const auto & output: kv.second) {
            if(output.getToActionId() == (int32_t)Step::Finish) {
                res.push_back(output);
            }
        }
    }
    return res;
}

bool StateMachine::execute(SessionPtr session, RequestPtr request)  {
    session->pushRequest(request);
    // now are we expecting this one
    
    
    
    if(session->getStatus().size() == 0)
        return firstCall(session); // we're in an uninitialized state ... this is a firstcall :)
    if(request->getTarget().target == ETargetAction::Error)
        return errorReceived(session, request);
    if(request->getTarget().target == ETargetAction::Interrupt)
        return interruptReceived(session);
    if(request->getTarget().target == ETargetAction::Status)
        return statusReceived(session);
    
    
    
    //! if execution level is 0, then it should be okay ... (means, it's probably error or interrupt or status.)
    if(request->getTarget().execution_level != 0 and
       request->getTarget().execution_level != session->getCurrentExecutionLevel()) {
        //! @todo add log: ignore request due to invalid execution level.
        BOOST_LOG_SEV(logger,Warn) << fingerprint(session) << request->logRequest() << " Unable to execute this request due to Inadequat execution level(requested: " << request->getTarget().execution_level << ", got: " << session->getCurrentExecutionLevel();
        return false;
    }
    
    if(request->getTarget().target == ETargetAction::Reply)
        return replyReceived(session, request);
    
    // this call wasn't expected.
    BOOST_LOG_SEV(logger, Error) << workflow_name << request->logRequest() << " Wasn't expecting this request now. This is totally unexpected. Ignoring it.";
    
    return false;
}

bool StateMachine::canExecuteAction(SessionPtr session, ActionPtr action, ErrorReport & er) {
    BOOST_LOG_SEV(logger,Trace) << fingerprint(session) << action->logAction() << " Checking ability to be executed now.";

    if(action->checkInputs(session, er)) {
        // guess it's okay ...
        if(action->canPerform(session, er)) {
            // now last check, all inputs must be done.
            if(inputs_map.count(action->getActionId()) > 0 ) {
                for(const auto & input: inputs_map.at(action->getActionId())) {
                    if(session->getStatus(input) != EExecutionStatus::Done) {
                        
                        BOOST_LOG_SEV(logger,Trace) << fingerprint(session) << action->logAction() << " Can't execute action right now. " << actions[input]->logAction() << " Not done.";
                        return false;
                    }
                }
            }
            return true;
        } else {
            BOOST_LOG_SEV(logger,Trace) << fingerprint(session) << action->logAction() << " Action refuses execution.";
        }
    } else {
        BOOST_LOG_SEV(logger,Trace) << fingerprint(session) << action->logAction() << " Can't execute action right now.";
    }
    return false;
}

bool StateMachine::canPendAction(SessionPtr session, ActionPtr action){
    ErrorReport er;
    if(action->checkInputs(session, er)) {
        return true;
    } else {
        BOOST_LOG_SEV(logger,Trace) << fingerprint(session) << action->logAction() << " Can't make action pending right now.";
    }
    return false;
}

std::string StateMachine::fingerprint(SessionPtr session) {
    std::stringstream str;
    str << "[" << workflow_name << "]" << session->getOriginalRequest()->logRequest() << " ";
    return str.str();
}

bool StateMachine::firstCall(SessionPtr session)  {
    // this is a first call, mark all actions as unplanned to begin with.
    BOOST_LOG_SEV(logger,Info) << fingerprint(session) << " First call from " << session->getOriginalRequest()->getReply() ;
    for(const auto & kv: actions)
        session->setStatus(kv.first, EExecutionStatus::Unplanned);
    
    // now map contexts.
    auto grouped_ctx = session->getOriginalRequest()->getContext();
    for(const auto & input: starters) {
        // empty (action/workflow) input means a "simple" next.
        // basically it's just ordering for execution plan.
        if(not input.getWorkflowInput().empty() and not input.getActionInput().empty())
            session->setInput(input.getActionId(), input.getActionInput(), grouped_ctx->getContext(input.getWorkflowInput()));
    }
    
    for(const auto & input: starters) {
        if(actions.count(input.getActionId()) == 0) {
            
            BOOST_LOG_SEV(logger, Error) << fingerprint(session) << " Aborting due to invalid workflow layout. Input requested doesn't exists.";
            return false;
        }
        auto act = actions.at(input.getActionId());
        ErrorReportPtr er(new ErrorReport());
        if(canExecuteAction(session, act, *er)) {
            addToNext(session, input.getActionId());
        } else if(canPendAction(session, act)) {
            addToPending(session, input.getActionId());
        } else {
            // can't schedule this action now.
        }
    }
    
    return executeAction(session, getNext(session));
}

bool StateMachine::interruptReceived(SessionPtr session)  {
    BOOST_LOG_SEV(logger,Info) << fingerprint(session) << " Interrupt Received !";
    session->getStatus().clear();
    session->getNexts().clear();
    session->getPendings().clear();
    
    for(const auto & kv: actions)
        session->setStatus(kv.first, EExecutionStatus::Unplanned);
    
    addToNext(session, (int32_t) Step::Interrupt);
    return executeAction(session, getNext(session));
}

bool StateMachine::errorReceived(SessionPtr session, RequestPtr request)  {
    
    if(actions.count(request->getTarget().action) > 0 ) {
        if(session->getStatus(request->getTarget().action) == EExecutionStatus::Waiting) {
            if(actions[request->getTarget().action]->canHandleError(session)) {
                
                BOOST_LOG_SEV(logger,Info) << fingerprint(session) << actions[request->getTarget().action]->logAction() << " Error Received ! but caught by action" ;
                actionExecuted(session, actions[request->getTarget().action]->replyReceived(session, request));
                return true;
            }
        }
    }
    
    
    BOOST_LOG_SEV(logger,Info) << fingerprint(session) << " Error Received !";
    session->getStatus().clear();
    session->getNexts().clear();
    session->getPendings().clear();
    
    for(const auto & kv: actions)
        session->setStatus(kv.first, EExecutionStatus::Unplanned);
    
    addToNext(session, (int32_t) Step::Error);
    return executeAction(session, getNext(session));
}

bool StateMachine::statusReceived(SessionPtr session)  {
    if(actions.count((int32_t)Step::Status)) {
        BOOST_LOG_SEV(logger,Info) << fingerprint(session) << " Status Received !";
        actions[(int32_t)Step::Status]->perform(session);
    } else {
        BOOST_LOG_SEV(logger,Info) << fingerprint(session) << " Status Ignored !";
    }
    return true;
}

bool StateMachine::replyReceived(SessionPtr session, RequestPtr request) {
    
    if(actions.count(request->getTarget().action) > 0 ) {
        if(session->getStatus(request->getTarget().action) == EExecutionStatus::Waiting) {
            BOOST_LOG_SEV(logger,Info) << fingerprint(session) << actions[request->getTarget().action]->logAction() << " Reply Received !";
            actionExecuted(session, actions[request->getTarget().action]->replyReceived(session, request));
            return true;
        } else {
            BOOST_LOG_SEV(logger, Warn) << fingerprint(session) << " Ignoring this reply, as currently running action doesn't expect a reply.";
        }
    } else {
         BOOST_LOG_SEV(logger, Warn) << fingerprint(session) << " Ignoring this reply, as currently running workflow doesn't expect a reply.";
    }
    return false;
}

bool StateMachine::executeAction(SessionPtr session, int32_t action_id)  {
    if(action_id == (int32_t) Step::Die) {
        auto wkf = getWorkflow().lock();
        auto ctrl = wkf->getController().lock();
        ctrl->requestFinished(session->getOriginalRequest());
        return true;
    }
    
    if(action_id == -1) {
        //! @todo add log, no action found.
        BOOST_LOG_SEV(logger, Debug) << fingerprint(session) << " No action to execute provided ... checking for promoted pending. ";

        
        int32_t next = getNext(session);
        ErrorReport er;
        if(next != -1 and
           canExecuteAction(session, actions[next], er)) {
            return executeAction(session, next);
        }
        
        BOOST_LOG_SEV(logger, Debug) << fingerprint(session) << " No action to execute provided ... checking ability to execute finish earlier ";

        if(canExecuteFinish(session)) {
            addToNext(session, (int32_t)Step::Finish);
            return executeAction(session, getNext(session));
        }
        
        BOOST_LOG_SEV(logger, Warn) << fingerprint(session) << " Truly nothing to execute, aborting execution.";
        
        addToNext(session, (int32_t) Step::Error);
        er = ErrorReport(session->getOriginalRequest()->getTarget(),
                         "statemachine.unexpected.noend", "No end of workflow available ... aborting");
        session->getLastRequest()->setErrorReport(ErrorReportPtr(new ErrorReport(er)));
        
        return executeAction(session, getNext(session));
    } else {
        // cleaning stuff ...
        removeFromNext(session, action_id);
        removeFromPending(session, action_id);
        
        // If we're switching to interrupt / error  / cleanup
        // then we must concidere that every actions in the normal flow hasn't been executed.
        // Actions in error flow shouldn't mingle with normal flow.
        if( action_id == (int32_t) Step::Interrupt or
           action_id == (int32_t) Step::Error or
           action_id == (int32_t) Step::Cleanup ) {
            session->getStatus().clear();
            session->getNexts().clear();
            session->getPendings().clear();
            
            for(const auto & kv: actions)
                session->setStatus(kv.first, EExecutionStatus::Unplanned);
            
            session->setStatus(action_id, EExecutionStatus::Planned);
        }
        
        if(actions.count(action_id) >0) {
            
            BOOST_LOG_SEV(logger, Debug) << fingerprint(session) << actions[action_id]->logAction() << " About to execute ...";
            
            auto result = actions[action_id]->perform(session);
            
            BOOST_LOG_SEV(logger, Debug) << fingerprint(session) << actions[action_id]->logAction() << " Executed: " << result.type;
            
            actionExecuted(session, result);

            return true;
        } else {
            BOOST_LOG_SEV(logger,Error) << fingerprint(session) << "Attempting to execute an unknown action: " << action_id;
            //! @todo add log: well that's an unknown action you want to execute.
            //! this should have been validated beforehand.
            return false;
        }
    }
 
}

void StateMachine::actionExecuted(SessionPtr session, const Result & result)  {
    auto action = actions[result.action_id];
    ErrorReport er;
    if(result.type == EType::Done and not action->checkOutputs(session, er)) {
        // move to error.
        BOOST_LOG_SEV(logger, Error) << fingerprint(session) << actions[result.action_id]->logAction() << " Failed its contracted outputs.";

        er = ErrorReport(session->getOriginalRequest()->getTarget(),
                         "action.contract.output", "Action failed it's outputs contract. Aborting");
        session->getLastRequest()->setErrorReport(ErrorReportPtr(new ErrorReport(er)));
        executeAction(session, (int32_t) Step::Error);
        return;
    }
    
    
    switch(result.type) {
        case EType::Done:
            session->setStatus(result.action_id, EExecutionStatus::Done),
            bindResults(session, result.action_id);
            executeAction(session, getNext(session));
            break;
        case EType::Async:
            session->setStatus(result.action_id, EExecutionStatus::Async);
            break;
        case EType::Finish:
            
            session->setStatus(result.action_id, EExecutionStatus::Done);
            bindResults(session, result.action_id);
            executeAction(session, (int32_t) Step::Finish);
            break;
        case EType::Wait:
            session->setStatus(result.action_id, EExecutionStatus::Waiting);
            break;
        case EType::Error: {
            auto last = session->getLastRequest();
            if(not last)
                BOOST_LOG_SEV(logger, Error) << fingerprint(session) <<actions[result.action_id]->logAction() << "Failed to find last request Oo";
            else
                last->setErrorReport(result.error);
            session->setStatus(result.action_id, EExecutionStatus::Done);
            executeAction(session, (int32_t) Step::Error);
            break;
        }
        default:
            BOOST_LOG_SEV(logger, Error) << fingerprint(session) << actions[result.action_id]->logAction() << " Unexpected result";

            session->getLastRequest()->setErrorReport(ErrorReportPtr(new
                                                      ErrorReport(session->getOriginalRequest()->getTarget(),
                                                                  "action.unknown.result", "Unexpected Action result. Aborting")));
            executeAction(session, (int32_t) Step::Error);
            break;
    }
    
}

void StateMachine::bindResults(SessionPtr session, int32_t action_id) {
    auto bindings = outputs[action_id];
    BOOST_LOG_SEV(logger, Debug) << fingerprint(session) << " Checking outputs of action: " << action_id;

    
    for(const auto & binding: bindings) {
        auto to_aid = binding.getToActionId();
        if(actions.count(to_aid) > 0) {
            BOOST_LOG_SEV(logger, Debug) << fingerprint(session) << " Checking binding from: " << actions[action_id]->logAction() << " to " << actions[to_aid]->logAction() << " ?";

            session->setInput(to_aid, binding.getToActionInput(),
                              session->getOutput(action_id, binding.getFromActionOutput()));
            
            auto action = actions[to_aid];
            
            bool isSkipped = false;
            // has it been skipped ?
            for(const auto & kv : session->getInputs()[to_aid]) {
                if(boost::dynamic_pointer_cast<SkipCtx>(kv.second)) {
                    removeFromNext(session, to_aid);
                    removeFromPending(session, to_aid);
                    session->setStatus(to_aid, EExecutionStatus::Skipped);
                    //! todo add log skipped action.
                    BOOST_LOG_SEV(logger, Debug) << fingerprint(session) << actions[to_aid]->logAction() << " Skipped";

                    isSkipped = true;
                    break;
                }
            }
            
            if(not isSkipped) {
                ErrorReport er;
                if(canExecuteAction(session, action, er)) {
                    removeFromPending(session, to_aid);
                    addToNext(session, to_aid);
                } else if(canPendAction(session, action)) {
                    removeFromNext(session, to_aid);
                    addToPending(session, to_aid);
                } else {
                    // well it's still unplanned.
                    removeFromNext(session, to_aid);
                    removeFromPending(session, to_aid);
                    session->setStatus(to_aid, EExecutionStatus::Unplanned);
                }
            }
            
            
        } else {
            if(to_aid == (int32_t)Step::Die) {
                addToNext(session, to_aid);
            } else
            BOOST_LOG_SEV(logger, Warn) << fingerprint(session) << " Unexpected absence of next action " << to_aid;
        }
    }
    
}

void StateMachine::actionAsyncFinished(SessionPtr session,const Result & result) {
    actionExecuted(session, result);
}

bool StateMachine::tryPromotePending(SessionPtr session) {
    auto pending = getNextPending(session);
    if(pending != -1) {
        // pending to promote :)
        
        BOOST_LOG_SEV(logger, Debug) << fingerprint(session) << actions[pending]->logAction() << " Promoting action to Planned !";
        
        removeFromPending(session, pending);
        addToNext(session, pending);
        return true;
    }
    return false;
}

int32_t StateMachine::getNext(SessionPtr session) {
    auto & nexts = session->getNexts();
    
    // No next found.
    if(nexts.size() == 0) {
        if(tryPromotePending(session))
            return getNext(session);
        return -1;
    }
    
    auto candidate_it = nexts.begin();
    int32_t candidate = * candidate_it;
    if(candidate == (int32_t)Step::Finish
       or candidate == (int32_t)Step::Cleanup
       or candidate == (int32_t)Step::Die) {
        // first ensure that there isn't any other request to ponder before finish / cleanup
        if(nexts.size() == 1) {
            // well there are no other options available.
            // but there might be something pending !
             if(tryPromotePending(session))
                 return getNext(session);
             else {
                // well no pending available.
                return candidate;
            }
        } else {
            // there cant be both finish, clean or die in the same execution stack.
            candidate_it++;
            return *candidate_it;
        }
            
    }
    
    return candidate;
}

int32_t StateMachine::getNextPending(SessionPtr session) {
    auto pendings = session->getPendings();
    
    // no pending found !
    if(pendings.size() == 0)
        return -1;
    
    auto candidate_it = pendings.begin();
    auto candidate = *candidate_it;
    if(candidate == (int32_t)Step::Finish
       or candidate == (int32_t)Step::Cleanup
       or candidate == (int32_t)Step::Die) {
        // first ensure that there isn't any other request to ponder before finish / cleanup
        if(pendings.size() == 1) {
                return candidate;
        } else {
            // there cant be both finish, clean or die in the same execution stack.
            candidate_it++;
            return *candidate_it;
        }
    }
    return candidate;
}

void StateMachine::addToNext(SessionPtr session, int32_t action_id) {
    session->getNexts().insert(action_id);
    session->setStatus(action_id, EExecutionStatus::Planned);
    
    if(action_id != (int32_t)Step::Die)
    BOOST_LOG_SEV(logger, Debug) << fingerprint(session) << actions[action_id]->logAction() << " Planned";
    else
        BOOST_LOG_SEV(logger, Debug) << fingerprint(session) << " Planned end of Workflow";
        
}

void StateMachine::addToPending(SessionPtr session, int32_t action_id) {
    session->getPendings().insert(action_id);
    session->setStatus(action_id, EExecutionStatus::Pending);
    
    BOOST_LOG_SEV(logger, Debug) << fingerprint(session) << actions[action_id]->logAction() << " Pending";
}

void StateMachine::removeFromNext(SessionPtr session, int32_t action_id) {
    session->getNexts().erase(action_id);
}

void StateMachine::removeFromPending(SessionPtr session, int32_t action_id) {
    session->getPendings().erase(action_id);
}

bool StateMachine::canExecuteFinish(SessionPtr session) {
    // Finish might get skiped sometimes, for good reason ( we wait for other thing to finish, or otherwise, the result of the workflow produce a Skip ...
    // Thus we've a special case here, ensuring that maybe, we could execute finish nonetheless.
    // requirements are :
    // All pendings and nexts are done.
    // No subqueries are being run.
    // Finish has been reached at least once ( one of its parent is done ).
    
    if(session->getNexts().size() != 0)
        return false;
    if(session->getPendings().size() != 0)
        return false;
    if(session->getSubQueries().size() != 0)
        return false;
    
    if(inputs_map.count((int32_t)Step::Finish) > 0) {
        // at least some do go to finish.
        for(auto input: inputs_map.at((int32_t) Step::Finish)) {
            BOOST_LOG_SEV(logger, Trace) << fingerprint(session) << " Finish expects some inputs from " << input << " : " << session->getStatus(input);
            if(session->getStatus(input) == EExecutionStatus::Done)
                return true;
        }
    }
    
    return false;
}

void StateMachine::save(boost::property_tree::ptree & root) const {
    boost::property_tree::ptree pactions;
    for(const auto & kv: actions) {
        boost::property_tree::ptree pitem;
        kv.second->save(pitem);
        pactions.add_child(std::to_string(kv.first), pitem);
    }
    root.add_child("actions", pactions);
    
    boost::property_tree::ptree poutputs;
    for(const auto & kv: outputs) {
        boost::property_tree::ptree plist;
        for(const auto & item: kv.second) {
            boost::property_tree::ptree pitem;
            item.save(pitem);
            plist.push_back(std::make_pair("", pitem));
        }
        poutputs.add_child(std::to_string(kv.first), plist);
    }
    root.add_child("outputs", poutputs);
    
    boost::property_tree::ptree pinputs;
    for(const auto & input: starters) {
        boost::property_tree::ptree pitem;
        input.save(pitem);
        pinputs.push_back(std::make_pair("", pitem));
    }
    root.add_child("inputs", pinputs);
    
    boost::property_tree::ptree pexposed;
    for(const auto & kv: exposed_properties) {
        boost::property_tree::ptree paction;
        for(const auto & ka: kv.second) {
            paction.put(ka.first, ka.second);
        }
        pexposed.add_child(std::to_string(kv.first), paction);
    }
    root.add_child("exposed", pexposed);
}

void StateMachine::load(const boost::property_tree::ptree & root) {
    auto coutputs = root.get_child_optional("outputs");
    std::map<int32_t, std::vector<OutputBinding> > toutputs;
    if(coutputs) {
        for(const auto & kv : *coutputs) {
            int32_t cid;
            std::stringstream str(kv.first.data());
            str >> cid;
            for(const auto & kvout: kv.second) {
                OutputBinding ob;
                ob.load(kvout.second);
                toutputs[cid].push_back(ob);
            }
        }
    }
    
    auto cactions = root.get_child_optional("actions");
    if(cactions) {
        for(const auto & kv: *cactions) {
            std::string type;
            GET_OPT(kv.second, type, std::string, "name");
            int32_t aid;
            std::stringstream str(kv.first.data());
            str >> aid;
            
            // generate a new Action.
            ActionPtr act = ActionFactory::create(type);
            
            if(act) {
                act->load(kv.second);
                addAction(aid, act, toutputs[aid]);
                std::stringstream str;
                for(const auto & out: toutputs[aid])
                    str << out << ", ";
                
                BOOST_LOG_SEV(logger, Trace) << " Built action: " << type <<", action id: " << act->getActionId() << ", action: " << act->logAction() << ": " << str.str();
            } else {
                BOOST_LOG_SEV(logger, Error) << " Failed to find appropriate Action for type: " << type;
            }
        }
    }
    
    auto cinputs = root.get_child_optional("inputs");
    if(cinputs) {
        for(const auto & kv: *cinputs) {
            InputBinding ib;
            ib.load(kv.second);
            starters.push_back(ib);
        }
    }
    
    auto cexposed = root.get_child_optional("exposed");
    if(cexposed) {
        exposed_properties.clear();
        for(const auto & kv: *cexposed) {
            std::stringstream str(kv.first.data());
            int32_t action_id;
            str >> action_id;
            for(const auto & ka: kv.second) {
                exposed_properties[action_id][ka.first.data()] = ka.second.data();
            }
        }
    }
}

bool StateMachine::finished(SessionPtr session) {
    return session->hasFinished();
}

ErrorReport StateMachine::validate() const {
    ErrorReport er;
    std::stringstream str;
    
    std::set<int32_t> unused_actions;
    for(const auto & kv: actions)
        unused_actions.insert(kv.first);
    
    // starters and outputs should link from and to known actions.
    // actions should at least have an a binding.
    
    for(const auto & input: starters) {
        unused_actions.erase(input.getActionId());
        if(actions.count(input.getActionId()) == 0) {
            str << input << " is invalid, targeted action doesn't exist" << std::endl;
        }
    }
    
    
    for(const auto & kv: outputs) {
        for(const auto & output: kv.second) {
            if(actions.count(output.getFromActionId()) == 0) {
                str << output << " is invalid, origin action doesn't exist" << std::endl;
            } else if (actions.count(output.getToActionId()) ==  0
                       and output.getToActionId() != (int32_t)Step::Die) {
                str << output << " is invalid, targeted action doesn't exist" << std::endl;
            } else {
                auto outputs = actions.at(output.getFromActionId())->getOutputs();
                
                auto ier = actions.at(output.getFromActionId())->expectOutput(output.getFromActionOutput());
                
                if(ier.isSet())
                    str << output << " " << ier.getErrorMessage()<< std::endl;
                else {
                    //! Die doesn't have any action, but that's okay :)
                    //! Finish must accept any inut.
                    if(output.getToActionId() == (int32_t)Step::Die or
                       output.getToActionId() == (int32_t)Step::Finish ) {
                        unused_actions.erase(output.getToActionId());
                        unused_actions.erase(output.getFromActionId());
                        continue;
                    }
                    
                    auto inputs = actions.at(output.getToActionId())->getInputs();
                    
                    auto oer = actions.at(output.getToActionId())->expectInput(output.getToActionInput());
                    
                    if(oer.isSet())
                        str << output << " " << oer.getErrorMessage()<< std::endl;
                    else {
                        unused_actions.erase(output.getToActionId());
                        unused_actions.erase(output.getFromActionId());
                    }
                }
            }
        }
    }
    
    if(str.str().empty())
        return er;
    
    er.setError("workflow.structure.invalid", str.str());
    return er;
}
