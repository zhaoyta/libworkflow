#include <service/client_manager.h>
#include <core/request.h>
#include <core/target.h>
#include <service/client.h>
#include <set>
#include <tools/timed.h>
#include <service/controller_manager.h>

static ClientManagerPtr instance;

ClientManager::ClientManager() :
    ActiveObject("ClientManager"),
    Logged("clt.mgr"),
    timer(new Timed()){
    
}

ClientManager::~ClientManager() {
    clients.clear();
}

ClientManagerPtr ClientManager::getInstance() {
    if(not instance)
        instance.reset(new ClientManager());
    return instance;
}

ClientPtr ClientManager::getClient(const boost::uuids::uuid & cid) {
    boost::interprocess::scoped_lock<boost::recursive_mutex> sl(*mutex);

    if(clients.count(cid) > 0)
        return clients.at(cid);
    return ClientPtr();
}

void ClientManager::addClient(ClientPtr clt) {
    boost::interprocess::scoped_lock<boost::recursive_mutex> sl(*mutex);

    if(clt){
        clients[clt->getId()] = clt;
    } else
        BOOST_LOG_SEV(logger, Warn) << "Attempting to add a NULL client";
}

void ClientManager::started() {
    timer->setIOService(getIOService());
    timer->reset();
    timer->setDuration(15000);
    timer->setTimeoutFunction((boost::bind(&ClientManager::checkClients, this)));
    timer->start();
}

void ClientManager::stopped() {
    for(const auto & kv: clients) {
        kv.second->disconnect();
    }
    clients.clear();
}

bool ClientManager::perform(RequestPtr request) {
    auto client = getClient(request->getTarget().client_id);
    if(client) {
        ErrorReport er;
        if(not client->sendRequest(request, er)) {
            auto rep = Request::createReply(request);
            if(rep->getTarget().target != ETargetAction::NoReply) {
                rep->getTarget().target = ETargetAction::Error;
                rep->setErrorReport(ErrorReportPtr(new ErrorReport(er)));
                ControllerManager::getInstance()->perform(rep);
                
            } else {
                BOOST_LOG_SEV(logger, Debug) << this << request->logRequest() << "Couldn't send this request, client refused it. Can't notify anyone about it.";
            }
            return false;
        } else {
            return true;
        }
    } else {
        BOOST_LOG_SEV(logger, Warn) << this << request->logRequest() << " Failed to find it's client. Can't forward the request.";
        return false;
    }
}

void ClientManager::checkClients(const boost::system::error_code & ec) {
    if(not ec) {
        boost::interprocess::scoped_lock<boost::recursive_mutex> sl(*mutex);

        std::set<boost::uuids::uuid> toRemove;
        for(const auto & kv: clients) {
            auto client = kv.second;
            if(client) {
                switch(client->getStatus()) {
                    case EClientStatus::Connected :
                    if(client->needHeartBeat()) {
                        BOOST_LOG_SEV(logger, Trace)<< this << client->logClient() << " Need an heartbeat ... checking it.";
                        client->heartbeat();
                    }
                    break;
                    case EClientStatus::HeartbeatExpected: {
                        if(client->heartbeatTimedOut()) {
                            BOOST_LOG_SEV(logger, Info)<< this << client->logClient() << " Heartbeat failed, kicking it.";

                            client->disconnect();
                            if(client->shouldDeleteClient()){
                                BOOST_LOG_SEV(logger, Info) << this << client->logClient() << " Should be deleted, dropping it.";

                                toRemove.insert(client->getId());
                            } else {
                                BOOST_LOG_SEV(logger, Info)<< this << client->logClient() << " Will try to reconnect it later.";

                            }
                        } else {
                            BOOST_LOG_SEV(logger, Debug)<< this << client->logClient() << " Heartbeat test ...";
                        }
                    }
                        break;
                    case EClientStatus::Disconnected: {
                    // weird to have an unconnected client but then we might as well ignore it.
                    BOOST_LOG_SEV(logger, Debug) << client << " Unexpected disconnected client during check. Trying reconnection";
                    client->connect();
                    }
                    default:
                        break;
                }
            } else {
                BOOST_LOG_SEV(logger, Warn) << " Found an unexpected NULL Client";
                toRemove.insert(kv.first);
            }
        }
        for(const auto & cid: toRemove) {
            removeClient(cid);
        }
    } else {
        BOOST_LOG_SEV(logger, Warn) << "Unexpected end of check.";
    }
}

void ClientManager::checkClients() {
    boost::system::error_code ec;
    getIOService()->dispatch(boost::bind(&ClientManager::checkClients, this, ec));
}

void ClientManager::removeClient(const boost::uuids::uuid & cid) {
    boost::interprocess::scoped_lock<boost::recursive_mutex> sl(*mutex);

    clients.erase(cid);
}

OSTREAM_HELPER_IMPL(ClientManager, obj) {
    out << "[ClientManager]";
    return out;
}