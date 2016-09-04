#include <core/workflow.h>
#include <core/session.h>
#include <tools/timed.h>
#include <core/state_machine.h>
#include <core/request.h>
#include <core/target.h>
#include <core/controller.h>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <service/controller_manager.h>


Workflow::Workflow(const std::string & name) :
    Jsonable(),
    boost::enable_shared_from_this<Workflow>(),
    Logged("wkf"),
    stateMachine(new StateMachine()),
    mutex(new boost::recursive_mutex()),
    name(name){
    
    stateMachine->init();
    
}

Workflow::~Workflow() {
    stateMachine.reset();
    controller.reset();
    sessions.clear();
}

void Workflow::setController(ControllerWPtr ctr) {
    controller = ctr;
}


RunningSession::RunningSession() :
    session(new Session()),
    timed(new Timed()){
    
}

RunningSession::~RunningSession() {
    
}

static RunningSession nullRunningSession;

const RunningSession & Workflow::getRunningSession(boost::uuids::uuid & id) const {
    boost::interprocess::scoped_lock<boost::recursive_mutex> sl(*mutex);

    if(sessions.count(id)) {
        return sessions.at(id);
    }
    return nullRunningSession;
}

bool Workflow::perform(RequestPtr request) {
    if(not request) {
        BOOST_LOG_SEV(logger, Error) << "[" << getName() << "]"  << "Ignored null request";
        return false;
    }
    
    boost::interprocess::scoped_lock<boost::recursive_mutex> sl(*mutex);
    BOOST_LOG_SEV(logger, Debug) << "[" << getName() <<"]"  << request->logRequest() << " Received request to process ...";

    if(canExecuteRequest(request)) {

        
        if(sessions.count(request->getId()) ==0 ){
            // No session found ! create a new one.
            auto & rsession = sessions[request->getId()];
            rsession.session.reset(new Session());
            //! initiate session.
            rsession.original = request;
            rsession.session->pushRequest(request);
            auto tmp_ctrl = controller.lock();
            rsession.timed->setIOService(tmp_ctrl->getIOService());
            rsession.timed->setDuration(timeout * 1000);
            rsession.timed->setTimeoutFunction(boost::bind<void>(&Workflow::requestTimedOut, this, request->getId()));
            BOOST_LOG_SEV(logger, Info) << "[" << getName() << "]"  << request->logRequest() << " Creating new session ...";
        }
        
        auto & rsession = sessions[request->getId()];
        
        if(shouldMakePending(request)) {
            addToPending(request);
            BOOST_LOG_SEV(logger, Debug) << "[" << getName() << "]"  << request->logRequest() << " Made pending ...";
            return true;
        }
        
        rsession.active = true;
        bool res = stateMachine->execute(rsession.session, request);
        rsession.active = false;

        if(stateMachine->finished(rsession.session)) {
            BOOST_LOG_SEV(logger, Info) << "[" << getName() << "]"  << request->logRequest() << " Closing session ...";
            //! @todo add log clear session
            sessions.erase(request->getId());
        }
        
        return res;
    } else {
        BOOST_LOG_SEV(logger, Warn) << "[" << getName() << "]"  << request->logRequest() << " Unable to execute request.";
        errorReply(request, new ErrorReport(request->getTarget(), "unable.to.execute", "Can't execute request"));
        return false;
    }
}



bool Workflow::shouldMakePending(RequestPtr request) {
    auto & rsession = sessions[request->getId()];

    if(rsession.active)
        return true;
    
    return false;
}

void Workflow::errorReply(RequestPtr request, ErrorReport * er) {
    auto reply = Request::createReply(request);
    reply->setErrorReport(ErrorReportPtr(er));
    ControllerManager::getInstance()->perform(reply);
}

void Workflow::addToPending(RequestPtr request) {
    auto & rsession = sessions[request->getId()];
    rsession.pendingRequests.push(request);
}

StateMachinePtr Workflow::getStateMachine() {
    stateMachine->setWorkflow(shared_from_this());
    
    return stateMachine;
}


bool Workflow::canExecuteRequest(RequestPtr request) {
    if(not request)
        return false;
    
    //! this is a new one.
    if(sessions.count(request->getId()) == 0 and request->getTarget().target == ETargetAction::DefaultAction)
        return true;
    
    auto type = request->getTarget().target;
    bool okay = type == ETargetAction::Interrupt
                or type == ETargetAction::Error
                or type == ETargetAction::Reply
                or type == ETargetAction::Status;
    
    if(sessions.count(request->getId()) == 1 and okay)
        return true;
    
    //! Allows status without session.
    if(sessions.count(request->getId()) == 0 and type == ETargetAction::Status)
        return true;
    
    if(sessions.count(request->getId()) == 1)
        BOOST_LOG_SEV(logger, Debug) << "[" << getName() << "]" << request->logRequest() << " Request targeted a running session without an appropriate Target";
    else
        BOOST_LOG_SEV(logger, Debug) << "[" << getName() << "]" << request->logRequest() << " Request targeted a new session without an appropriate Target";
    BOOST_LOG_SEV(logger, Debug) << "[" << getName() << "]" << request->getTarget();
    return false;
}

void Workflow::requestTimedOut(boost::uuids::uuid id) {
    boost::interprocess::scoped_lock<boost::recursive_mutex> sl(*mutex);

    if(sessions.count(id) != 0) {
        auto session = sessions.at(id);
        auto target = Target(session.session->getOriginalRequest()->getTarget());
        BOOST_LOG_SEV(logger, Info) << getName() << session.session->getOriginalRequest()->logRequest() << " Request Timed out ... interrupting ...";

        target.target = ETargetAction::Interrupt;
        auto req = RequestPtr(new Request(target));
        perform(req);
    } else {
        BOOST_LOG_SEV(logger, Debug) << getName() << " A time out fired, but no request with this id found ...";
    }
}

void Workflow::setTimeout(double d) {
    timeout = d;
}

std::string Workflow::getName() const {
    return name;
}



void Workflow::save(boost::property_tree::ptree & root) const {
    
}

void Workflow::load(const boost::property_tree::ptree & root) {
    
}

OSTREAM_HELPER_IMPL(Workflow, obj) {
    out << "[Workflow] name: " << obj.getName();
    return out;
}
