#include <service/actor.h>
#include <tools/error_report.h>
#include <core/request.h>
#include <core/target.h>

Actor::Actor(const std::string & name) : Client(), ActiveObject(name), Logged("actor"){
}

Actor::~Actor() {}

ActorPtr Actor::shared_from_this() {
    return boost::dynamic_pointer_cast<Actor>(ActiveObject::shared_from_this());
}

void Actor::publishRequest(RequestPtr req) {
    ErrorReport er;
    if(not receiveNewRequest(req, er)) {
        // Well it probably has already been logged.
    }
    
}
//! Same, except that it'll reply with error :)
void Actor::replyError(RequestPtr req, const ErrorReport & er) {
    if(req) {
        auto rep = Request::createReply(req);
        if(rep->getTarget().target != ETargetAction::NoReply) {
            rep->getTarget().target = ETargetAction::Error;
            rep->setErrorReport(ErrorReportPtr(new ErrorReport(er)));
        } else {
            BOOST_LOG_SEV(logger, Warn)  << logActor() << " Well attempting to reply to a NoReply request";
        }
    }
}

bool Actor::hasPendingRequest() const {
    boost::interprocess::scoped_lock<boost::recursive_mutex> sl(*mutex);

    return pending_requests.size() > 0 ;
}

RequestPtr Actor::dequeuePendingRequest() {
    boost::interprocess::scoped_lock<boost::recursive_mutex> sl(*mutex);

    
    if(pending_requests.size() > 0) {
    auto res = pending_requests.front();
    pending_requests.pop();
    return res;
    }
    return RequestPtr();
}

void Actor::newRequestReceived() {
    BOOST_LOG_SEV(logger, Error) << logActor() << " A request has been received and there is noone to take it";
}

void Actor::disconnect() {
    terminate();
    Client::disconnect();
}

void Actor::connect() {
    start();
    Client::connect();
}

RequestPtr Actor::peekNextRequest() {
    boost::interprocess::scoped_lock<boost::recursive_mutex> sl(*mutex);

    if(pending_requests.size() > 0)
    return pending_requests.front();
    return RequestPtr();
}

std::string Actor::logActor() const {
    std::stringstream str;
    str << "[A " << getName() << "]";
    return str.str();
}

bool Actor::canSendRequest(RequestPtr, ErrorReport &) {
    return true;
}

bool Actor::doSendRequest(RequestPtr req) {
    getIOService()->dispatch(boost::bind<void>([&](RequestPtr req) -> void {
        pending_requests.push(req);
    }, req));
    getIOService()->dispatch(boost::bind<void>([&](RequestPtr req) -> void {
        newRequestReceived();
    }, req));
    return true;
}

void Actor::started() {
    BOOST_LOG_SEV(logger, Info) << logActor() << " Actore base started.";
}

void Actor::stopped() {
    
}