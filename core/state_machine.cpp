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


StateMachine::StateMachine() : Jsonable(), Logged("wkf.sm") {
        
        // define basic workflow.
        
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

StateMachine::~StateMachine() {
    
}

WorkflowPtr StateMachine::getWorkflow() {
    return workflow;
}

void StateMachine::setWorkflow(WorkflowPtr wk) {
    workflow = wk;
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
        //! @todo add log: reject action due to action_id being negative unduely.
        return;
    }
    
    actions[action_id] = action;
    outputs[action_id] = bindings;
    // ensure that we know what actions are necessary for execution of another one.
    for(auto & output: bindings) {
        inputs_map[output.getToActionId()].insert(action_id);
    }
}

void StateMachine::addInput(const InputBinding & input) {
    starters.push_back(input);
}

const std::map<int32_t, ActionPtr> StateMachine::getActions() const {
    return actions;
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
        return false;
    }
    
    if(request->getTarget().target == ETargetAction::Reply)
        return replyReceived(session, request);
    
    // this call wasn't expected.
    //! @todo add log
    
    return false;
}

bool StateMachine::canExecuteAction(SessionPtr session, ActionPtr action, ErrorReport & er) {
    if(action->checkInputs(session, er)) {
        // guess it's okay ...
        if(action->canPerform(session, er)) {
            // now last check, all inputs must be done.
            for(const auto & input: inputs_map.at(action->getActionId())) {
                if(session->getStatus(input) != EExecutionStatus::Done)
                    return false;
            }
            return true;
        }
    }
    return false;
}

bool StateMachine::canPendAction(SessionPtr session, ActionPtr action){
    ErrorReport er;
    if(action->checkInputs(session, er)) {
        return true;
    }
    return false;
}

bool StateMachine::firstCall(SessionPtr session)  {
    // this is a first call, mark all actions as unplanned to begin with.
    for(const auto & kv: actions)
        session->setStatus(kv.first, EExecutionStatus::Unplanned);
    
    // now map contexts.
    auto grouped_ctx = session->getOriginalRequest()->getCastedContext<GroupedCtx>();
    for(const auto & input: starters) {
        session->setInput(input.getActionId(), input.getActionInput(), grouped_ctx->getContext(input.getWorkflowInput()));
    }
    
    for(const auto & input: starters) {
        if(actions.count(input.getActionId()) == 0) {
            //! @todo add log here, not all input are ready.
            //! @todo reply with error.
            //! @todo maybe this isn't the place to check validity of the workflow .... it should be checked upon insertion in controller ... :)
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
                actionExecuted(session, actions[request->getTarget().action]->replyReceived(session, request));
                return true;
            }
        }
    }
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
        actions[(int32_t)Step::Status]->perform(session);
    }
    return true;
}

bool StateMachine::replyReceived(SessionPtr session, RequestPtr request) {
    if(actions.count(request->getTarget().action) > 0 ) {
        if(session->getStatus(request->getTarget().action) == EExecutionStatus::Waiting) {
            actionExecuted(session, actions[request->getTarget().action]->replyReceived(session, request));
            return true;
        } else {
            //! @todo add log, action doesn't expect reply.
        }
    } else {
        //! @todo add log, unknown action requested.
    }
    return false;
}

bool StateMachine::executeAction(SessionPtr session, int32_t action_id)  {
    if(action_id == (int32_t) Step::Die)
        return true;
    
    if(action_id != -1) {
        //! @todo add log, no action found.
        
        int32_t next = getNext(session);
        ErrorReport er;
        if(next != -1 and
           canExecuteAction(session, actions[next], er)) {
            return executeAction(session, next);
        }
        
        if(canExecuteFinish(session)) {
            addToNext(session, (int32_t)Step::Finish);
            
            //! @todo add log Forcefully executing finish
            return executeAction(session, getNext(session));
        }
        
        //! @todo add log: Nothing to execute ...
        addToNext(session, (int32_t) Step::Error);
        // might be nice to add an error here ...
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
            
            actionExecuted(session, actions[action_id]->perform(session));
            
            return true;
        } else {
            //! @todo add log: well that's an unknown action you want to execute.
            //! this should have been validated beforehand.
            return false;
        }
    }
 
}

void StateMachine::actionExecuted(SessionPtr session, const Result & result)  {
    auto action = actions[result.action_id];
    ErrorReport er;
    if(not action->checkOutputs(session, er)) {
        // move to error.
        
        //! @todo add log: Action didn't respect it's contracted outputs.
        er = ErrorReport(session->getOriginalRequest()->getTarget(),
                         "action.contract.output", "Action failed it's outputs contract. Aborting");
        session->getLastRequest()->setErrorReport(ErrorReportPtr(new ErrorReport(er)));
        executeAction(session, (int32_t) Step::Error);
    }
    
    
    switch(result.type) {
        case Result::Done:
            bindResults(session, result.action_id);
            session->setStatus(result.action_id, EExecutionStatus::Done),
            executeAction(session, getNext(session));
            break;
        case Result::Async:
            session->setStatus(result.action_id, EExecutionStatus::Async);
            break;
        case Result::Finish:
            bindResults(session, result.action_id);
            session->setStatus(result.action_id, EExecutionStatus::Done);
            executeAction(session, (int32_t) Step::Finish);
            break;
        case Result::Wait:
            session->setStatus(result.action_id, EExecutionStatus::Waiting);
            break;
        case Result::Error:
            session->getLastRequest()->setErrorReport(result.error);
            bindResults(session, result.action_id);
            session->setStatus(result.action_id, EExecutionStatus::Done);
            executeAction(session, (int32_t) Step::Error);
            break;
        default:
            session->getLastRequest()->setErrorReport(ErrorReportPtr(new
                                                      ErrorReport(session->getOriginalRequest()->getTarget(),
                                                                  "action.unknown.result", "Unexpected Action result. Aborting")));
            executeAction(session, (int32_t) Step::Error);
            break;
    }
    
}

void StateMachine::bindResults(SessionPtr session, int32_t action_id) {
    auto bindings = outputs[action_id];
    for(const auto & binding: bindings) {
        auto to_aid = binding.getToActionId();
        if(actions.count(to_aid) > 0) {
            session->setInput(to_aid, binding.getToActionInput(),
                              session->getOutput(action_id, binding.getFromActionOutput()));
            
            auto action = actions[to_aid];
            
            bool isSkipped = false;
            // has it been skipped ?
            for(const auto & kv : session->getInputs()[action_id]) {
                if(boost::dynamic_pointer_cast<SkipCtx>(kv.second)) {
                    removeFromNext(session, to_aid);
                    removeFromPending(session, to_aid);
                    session->setStatus(to_aid, EExecutionStatus::Skipped);
                    //! todo add log skipped action.
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
            //! @todo add log here: unexpected absence of next.
            // this again is a validation issue, should be handled elsewhere.
        }
    }
    
}

void StateMachine::actionAsyncFinished(SessionPtr session,const Result & result) {
    actionExecuted(session, result);
}

int32_t StateMachine::getNext(SessionPtr session) {
    auto nexts = session->getNexts();
    
    // No next found.
    if(nexts.size() == 0)
        return -1;
    
    auto candidate_it = nexts.begin();
    int32_t candidate = * candidate_it;
    if(candidate == (int32_t)Step::Finish
       or candidate == (int32_t)Step::Cleanup
       or candidate == (int32_t)Step::Die) {
        // first ensure that there isn't any other request to ponder before finish / cleanup
        if(nexts.size() == 1) {
            // well there are no other options available.
            // but there might be something pending !
            auto pending = getNextPending(session);
            if(pending >= 0) {
                // pending to promote :)
                
                //! @todo add log promotion !
                removeFromPending(session, pending);
                session->setStatus(pending, EExecutionStatus::Planned);
                return pending;
            } else {
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
}

void StateMachine::addToPending(SessionPtr session, int32_t action_id) {
    session->getPendings().insert(action_id);
    session->setStatus(action_id, EExecutionStatus::Pending);
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
            if(session->getStatus(input) == EExecutionStatus::Done)
                return true;
        }
    }
    
    return false;
}

void StateMachine::save(boost::property_tree::ptree & root) const {
    
}

void StateMachine::load(const boost::property_tree::ptree & root) {
    
}

bool StateMachine::finished(SessionPtr session) {
    return session->hasFinished();
}