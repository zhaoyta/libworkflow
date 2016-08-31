#include <service/client_manager.h>
#include <service/client.h>
#include <set>
#include <tools/timed.h>

static ClientManagerPtr instance;

ClientManager::ClientManager() :
    ActiveObject("ClientManager"),
    timer(new Timed()) {
    setNamespace("clt.mgr");
    
}

ClientManager::~ClientManager() {
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
                        BOOST_LOG_SEV(logger, Trace) << client << " Need an heartbeat ... checking it.";
                        client->heartbeat();
                    }
                    break;
                    case EClientStatus::HeartbeatExpected: {
                        if(client->heartbeatTimedOut()) {
                            BOOST_LOG_SEV(logger, Info) << client << " Heartbeat failed, kicking it.";

                            client->disconnect();
                            if(client->shouldDeleteClient()){
                                BOOST_LOG_SEV(logger, Info) << client << " Should be deleted, dropping it.";

                                toRemove.insert(client->getId());
                            } else {
                                BOOST_LOG_SEV(logger, Info) << client << " Will try to reconnect it later.";

                            }
                        } else {
                            BOOST_LOG_SEV(logger, Debug) << client << " Heartbeat test ...";
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