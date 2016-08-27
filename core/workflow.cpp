#include <core/workflow.h>
#include <core/session.h>
#include <tools/timed.h>
#include <core/state_machine.h>
#include <core/controller.h>


Workflow::Workflow(const std::string & name) :
    stateMachine(new StateMachine()),
    name(name){
    
}

Workflow::~Workflow() {
    
}

void Workflow::setController(ControllerPtr ctr) {
    controller = ctr;
}


Workflow::RunningSession::RunningSession() :
    original(request),
    session(new Session()),
    timed(new Timed()){
    
}

Workflow::RunningSession::~RunningSession() {
    
}

bool Workflow::perform(RequestPtr request) {
    if(canExecuteRequest(request)) {
        auto & rsession = sessions[request->getId()];
        
        if(sessions.count(request->getId()) ==0 ){
            // No session found ! create a new one.
            rsession.session.reset(new Session());
            //! initiate session.
            rsession.session->pushRequest(request);
            rsession.timed->setIOService(controller->getIOService());
            rsession.timed->setDuration(timeout * 1000);
            rsession.timed->setTimeoutFunction(boost::bind<void>(&Workflow::requestTimedOut, this, request->getId()));
        }
        
        if(shouldMakePending(request)) {
            addToPending(request);
            return true;
        }
        
        bool res = stateMachine->perform(rsession.session);
        
        if(stateMachine->finished(rsession.session)) {
            //! @todo add log clear session
            sessions.erase(request->getId());
        }
        
        return res;
    } else {
        errorReply(request, new ErrorReport(request->getTarget(), "unable.to.execute", "Can't execute request"));
        return false;
    }
}

StateMachinePtr Workflow::getStateMachine() {
    return stateMachine;
}


bool Workflow::canExecuteRequest(RequestPtr request) {
    if(not request)
        return false;
    
    //! this is a new one.
    if(sessions.count(request->getId()) == 0 and request->getTarget().type == Target::DefaultAction)
        return true;
    
    auto type = request->getTarget().type;
    bool okay = type == Target::Interrupt
                or type == Target::Error
                or type == Target::Reply
                or type == Target::Status;
    
    if(sessions.count(request->getId()) == 1 and okay)
        return true;
    
    //! Allows status without session.
    if(sessions.count(request->getId()) == 0 and type == Target::Status)
        return true;
    
    //! @todo add log here as to why request has been rejected.
    return false;
}

void Workflow::requestTimedOut(boost::uuids::uuid id) {
    if(sessions.count(id) != 0) {
        auto session = sessions.at(id);
        auto target = Target(session->getOriginalRequest()->getTarget());
        target.type = Target::Interrupt;
        auto req = RequestPtr(new Request(target));
        perform(req);
    } else {
        //! @todo add log as to timeout failed to find a target.
    }
}

void Workflow::setTimeout(double d) {
    timeout = d;
}