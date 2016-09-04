#include <core/action_wrappers.h>
#include <core/session.h>
#include <core/request.h>
#include <tools/error_report.h>
#include <core/action.h>
#include <core/result.h>
#include <service/controller_manager.h>
#include <core/state_machine.h>
#include <core/context.h>

ActionWrapper::ActionWrapper( ActionPtr wrapped) : Action(wrapped->getName()),
wrapped(wrapped) {
    
}

ActionWrapper::~ActionWrapper() {
    
}

bool ActionWrapper::checkInputs(SessionPtr session, ErrorReport & er) const {
    return wrapped->checkInputs(session, er);
}

bool ActionWrapper::checkOutputs(SessionPtr session, ErrorReport & er) const {
    return wrapped->checkOutputs(session, er);
}

Result ActionWrapper::perform(SessionPtr session) const {
    wrapPerform(session);
    auto res = wrapped->perform(session);
    wrapPostPerform(session);
    return res;
}

Result ActionWrapper::replyReceived(SessionPtr session, RequestPtr request) const {
    wrapReplyReceived(session, request);
    auto res = wrapped->replyReceived(session, request);
    wrapPostReplyReceived(session, request);
    return res;
}

bool ActionWrapper::canPerform(SessionPtr session, ErrorReport & er) const {
    return wrapped->canPerform(session, er);
}

bool ActionWrapper::canHandleError(SessionPtr session) const {
    return wrapped->canHandleError(session);
}

void ActionWrapper::setActionId(int32_t action_id) {
    return wrapped->setActionId(action_id);
}

int32_t ActionWrapper::getActionId() const {
    return wrapped->getActionId();
}


const std::string & ActionWrapper::getName() const {
    return wrapped->getName();
}

void ActionWrapper::setName(const std::string & n) {
    wrapped->setName(n);
}

PropertySetPtr ActionWrapper::properties() {
    return wrapped->properties();
}

void ActionWrapper::wrapPerform(SessionPtr) const{
    
}

void ActionWrapper::wrapPostPerform(SessionPtr) const {
    
}

void ActionWrapper::wrapReplyReceived(SessionPtr, RequestPtr) const {
    
}

void ActionWrapper::wrapPostReplyReceived(SessionPtr, RequestPtr) const {
    
}


InterruptWrapper::InterruptWrapper(ActionPtr action) : ActionWrapper(action) {}
void InterruptWrapper::wrapPerform(SessionPtr session) const {
    session->upCurrentExecutionLevel();
    for(const auto & target: session->getSubQueries()) {
        auto req = RequestPtr(new Request(target));
        //! this is redondant, as Interrupt are always accepted, no matter what EL.
        req->getTarget().execution_level = 0;
        req->getTarget().target = ETargetAction::Interrupt;
        //! this ensure we don't get any intel as to what happend from the interrupted request.
        req->getReply().target = ETargetAction::NoReply;
        req->setRequestId(session->getOriginalRequest()->getRequestId());
        //! @todo Get controller manager, execute action.
    }
}

FinishWrapper::FinishWrapper(ActionPtr action): InterruptWrapper(action) {}
void FinishWrapper::wrapPerform(SessionPtr session) const {
    InterruptWrapper::wrapPerform(session);
    auto reply = Request::createReply(session->getOriginalRequest());
    
    // find contexts ...
    // bind them with result.
    
    auto ctx = GroupedCtxPtr( new GroupedCtx());
    
    for(const auto & output: getStateMachine().lock()->getExpectedOutput()) {
        ctx->setContext(output, getInput(session, output));
    }
    
    reply->setContext(ctx);
    
    
    BOOST_LOG_SEV(logger, Info) << fingerprint(session) << " Replying to " << reply->getTarget();
    
    ControllerManager::getInstance()->perform(reply);
}

ErrorWrapper::ErrorWrapper(ActionPtr action) : InterruptWrapper(action) {}
void ErrorWrapper::wrapPerform(SessionPtr session)const  {
    InterruptWrapper::wrapPerform(session);
    
    auto er = session->getLastRequest()->getErrorReport();
    auto reply = Request::createReply(session->getOriginalRequest());
    reply->setErrorReport(ErrorReportPtr(new ErrorReport(session->getOriginalRequest()->getTarget(), er)));
    ControllerManager::getInstance()->perform(reply);
}

CleanupWrapper::CleanupWrapper(ActionPtr action): InterruptWrapper(action) {}
void CleanupWrapper::wrapPerform(SessionPtr session) const {
    InterruptWrapper::wrapPerform(session);
}

