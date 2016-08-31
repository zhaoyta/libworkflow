#include <service/client.h>
#include <core/target.h>
#include <core/request.h>
#include <service/controller_manager.h>
#include <tools/error_report.h>

BEGIN_ENUM_IMPL(ClientStatus) {
    {"Connected",(uint32_t)EClientStatus::Connected},
{"HeartbeatExpected",(uint32_t)EClientStatus::HeartbeatExpected},
    {"Disconnected",(uint32_t)EClientStatus::Disconnected},
};
END_ENUM_IMPL(ClientStatus);

Client::Client() : Logged("clt"), status(EClientStatus::Disconnected) {
    
}
Client::~Client() {
    
}

boost::uuids::uuid Client::getId() const {
    return client_id;
}

std::string Client::logClient() const {
    std::stringstream str;
    str << "[C " << shortId(client_id) << "]";
    return str.str();
}

EClientStatus Client::getStatus() const {
    return status;
}

void Client::setStatus(EClientStatus s, bool force ) {
    if(s != status) {
        auto old_s = status;
        status = s;
        if(not force)
            internalStatusChanged(old_s, s);
    }
}

bool Client::needHeartBeat() const {
    return false;
}

void Client::heartbeat() {
    
}

bool Client::heartbeatTimedOut() {
    return false;
}

bool Client::shouldDeleteClient() {
    return false;
}

void Client::connect() {
    setStatus(EClientStatus::Connected);
}

void Client::disconnect() {
    setStatus(EClientStatus::Disconnected);
}

bool Client::receiveNewRequest(RequestPtr req, ErrorReport & er) {
    if(isReceivedRequestAllowed(req, er)) {
        if(req->getTarget().target == ETargetAction::Reply) {
            // this is a reply ... check it.
            if(sentRequests.count(req->getRequestId()) > 0){
                BOOST_LOG_SEV(logger, Debug) << logClient() << " Expected reply returned !";
                sentRequests.erase(req->getId());
            }
        } else {
            // we're handling a new request from our host. Storing it.
            // only if host expect a reply.
            if(req->getReply().target == ETargetAction::Reply)
                receivedRequests[req->getId()] = req;
        }
        doReceiveRequest(req);
        
        return true;
    }
    return false;
}

bool Client::sendRequest(RequestPtr req, ErrorReport & er) {
    if(canSendRequest(req, er)) {
        if(req->getTarget().target == ETargetAction::DefaultAction) {
            // store it, well only if we expect a reply from it.
            if(req->getReply().target != ETargetAction::NoReply)
                sentRequests[req->getId()] = req;
        } else if(req->getTarget().target == ETargetAction::Reply){
            // we're replying to a request.
            if(receivedRequests.count(req->getId()) > 0) {
                receivedRequests.erase(req->getId());
            }
        }
        
        doSendRequest(req);
        
        return true;
    }
    return false;
}

void Client::statusChanged(EClientStatus, EClientStatus) {
    
}


void Client::internalStatusChanged(EClientStatus from, EClientStatus to) {
    if(to == EClientStatus::Disconnected) {
        BOOST_LOG_SEV(logger, Warn) << logClient() << " Client got disconnected, interrupting pending requests";
        auto ctrl = ControllerManager::getInstance() ;
        for(const auto & kv: sentRequests) {
            auto rep = Request::createReply(kv.second);
            rep->getTarget().target = ETargetAction::Interrupt;
            rep->getReply().target = ETargetAction::NoReply;
            BOOST_LOG_SEV(logger, Warn) << logClient() << rep->logRequest() << " Interrupting request ...";
            ctrl->perform(rep);
        }
        
        for(const auto & kv: receivedRequests) {
            auto rep = RequestPtr(new Request(kv.second->getTarget()));
            rep->getTarget().target = ETargetAction::Interrupt;
            rep->getReply().target = ETargetAction::NoReply;
            BOOST_LOG_SEV(logger, Warn) << logClient() << rep->logRequest() << " Interrupting request ...";
            ctrl->perform(rep);
        }
    }
    statusChanged(from,to);
}


bool Client::isReceivedRequestAllowed(RequestPtr, ErrorReport & er) {
    return true;
}

bool Client::canSendRequest(RequestPtr, ErrorReport & er) {
    er.setError("client.cant.send", "This is a default client, and has nothing to send");
    return false;
}

void Client::doSendRequest(RequestPtr ) {
    
}

void Client::doReceiveRequest(RequestPtr req) {
    ControllerManager::getInstance()->perform(req);
}
