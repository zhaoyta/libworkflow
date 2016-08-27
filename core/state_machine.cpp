#include <core/state_machine.h>
#include <core/request.h>
#include <core/session.h>
#include <core/action.h>
#include <core/bindings.h>
#include <core/workflow.h>


StateMachine::StateMachine(WorkflowPtr workflow) : Jsonable(),
    workflow(workflow) {
    
}

StateMachine::~StateMachine() {
    
}

bool StateMachine::execute(SessionPtr session, RequestPtr request)  {
    
}

void StateMachine::addAction(int32_t action_id, ActionPtr action, const std::vector<OutputBinding> & bindings) {
    
}

void StateMachine::addInput(const InputBinding & input) {
    
}

const std::map<int32_t, ActionPtr> StateMachine::getActions() const {
    
}

bool StateMachine::firstCall(SessionPtr session)  {
    
}

bool StateMachine::interruptReceived(SessionPtr session)  {
    
}

bool StateMachine::errorReceived(SessionPtr session)  {
    
}

bool StateMachine::statusReceived(SessionPtr session, RequestPtr request)  {
    
}

bool StateMachine::replyReceived(SessionPtr session, RequestPtr request) {
    
}

void StateMachine::execute(SessionPtr, int32_t action)  {
    
}

void StateMachine::actionExecuted(SessionPtr, const Result & result)  {
    
}

int32_t StateMachine::getNext(SessionPtr) {
    
}

int32_t StateMachine::getNextPending(SessionPtr) {
    
}

void StateMachine::addToNext(SessionPtr, int32_t) {
    
}

void StateMachine::addToPending(SessionPtr, int32_t) {
    
}

void StateMachine::removeFromNext(SessionPtr, int32_t) {
    
}

void StateMachine::removeFromPending(SessionPtr, int32_t) {
    
}

bool StateMachine::canExecuteFinish(SessionPtr) {
    
}

void StateMachine::save(boost::property_tree::ptree & root) const {
    
}

void StateMachine::load(const boost::property_tree::ptree & root) {
    
}

bool StateMachine::finished(SessionPtr session) {
    return session->hasFinished();
}