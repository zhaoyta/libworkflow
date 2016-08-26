#include <core/action.h>
#include <core/result.h>

ActionWrapper::ActionWrapper( ActionPtr wrapper) : Action(wrapper->getName()),
wrapper(wrapper) {
    
}

ActionWrapper::~ActionWrapper() {
    
}

bool ActionWrapper::checkInputs(SessionPtr session, ErrorReport & er) const {
    return wrapper->checkInputs(session, er);
}

bool ActionWrapper::checkOutputs(SessionPtr session, ErrorReport & er) const {
    return wrapper->checkOutputs(session, er);
}

Result ActionWrapper::perform(SessionPtr session) const {
    wrapPerform(session);
    auto res = wrapper->perform(session);
    wrapPostPerform(session);
    return res;
}

Result ActionWrapper::replyReceived(SessionPtr session, RequestPtr request) const {
    wrapReplyReceived(session, request);
    auto res = wrapper->replyReceived(session, request);
    wrapPostReplyReceived(session, request);
    return res;
}

bool ActionWrapper::canPerform(SessionPtr session, ErrorReport & er) const {
    return wrapper->canPerform(session, er);
}

bool ActionWrapper::canHandleError(SessionPtr session) const {
    return wrapper->canHandleError(session);
}

void ActionWrapper::setActionId(int32_t action_id) {
    return wrapper->setActionId(action_id);
}

int32_t ActionWrapper::getActionId() const {
    return wrapper->getActionid();
}

void ActionWrapper::wrapPerform(SessionPtr) {
    
}

void ActionWrapper::wrapPostPerform(SessionPtr) {
    
}

void ActionWrapper::wrapReplyReceived(SessionPtr, RequestPtr) {
    
}

void ActionWrapper::wrapPostReplyReceived(SessionPtr, RequestPtr) {
    
}


InterruptWrapper::InterruptWrapper(ActionPtr action) : ActionWrapper(action) {}
void InterruptWrapper::wrapPerform(SessionPtr session) {
    
}

FinishWrapper::FinishWrapper(ActionPtr action): ActionWrapper(action) {}
void FinishWrapper::wrapPerform(SessionPtr session) {

}

ErrorWrapper::ErrorWrapper(ActionPtr action) : ActionWrapper(action) {}
void ErrorWrapper::wrapPerform(SessionPtr session) {
}

CleanupWrapper::CleanupWrapper(ActionPtr action): ActionWrapper(action) {}
void CleanupWrapper::wrapPerform(SessionPtr session) {
    
}

