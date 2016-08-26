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
    session->upCurrentExecutionLevel();
    for(const auto & target: session->getSubQueries()) {
        auto req = RequestPtr(new Request(target));
        //! this is redondant, as Interrupt are always accepted, no matter what EL.
        req->getTarget().execution_level = 0;
        req->getTarget().target = Target::Interrupt;
        //! this ensure we don't get any intel as to what happend from the interrupted request.
        req->getReply().target = Target::NoReply;
        req->setRequestId(session->getOriginalRequest()->getRequestId());
        //! @todo Get controller manager, execute action.
    }
}

FinishWrapper::FinishWrapper(ActionPtr action): InterruptWrapper(action) {}
void FinishWrapper::wrapPerform(SessionPtr session) {
    InterruptWrapper::wrapPerform(session);
    auto reply = Request::reply(session->getOriginalRequest());
}

ErrorWrapper::ErrorWrapper(ActionPtr action) : InterruptWrapper(action) {}
void ErrorWrapper::wrapPerform(SessionPtr session) {
    InterruptWrapper::wrapPerform(session);
    
    auto er = session->getLastRequest()->getErrorReport();
    auto reply = Request::reply(session->getOriginalRequest());
    reply->setErrorReport(session->getOriginalRequest()->getTarget(),
                          er);
}

CleanupWrapper::CleanupWrapper(ActionPtr action): InterruptWrapper(action) {}
void CleanupWrapper::wrapPerform(SessionPtr session) {
    InterruptWrapper::wrapPerform(session);
}

