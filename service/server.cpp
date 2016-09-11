#include <service/server.h>
#include <service/controller_manager.h>
#include <service/client_manager.h>
#include <core/controller.h>
#include <service/actor.h>
#include <boost/bind.hpp>

Server::Server() : ActiveObject("Server", 1,false), controllers_expected(0), clients_expected(0) {
    
}

Server::~Server() {
    
}

void Server::addController(ControllerPtr ctr) {
    controllers_expected++;
    ctr->setStartedFunction(boost::bind(&Server::decreaseControllersCounter, this, _1));
    ctr->start();
    ControllerManager::getInstance()->registerController(ctr);
}

void Server::addActor(ActorPtr act) {
    clients_expected++;
    act->setStartedFunction(boost::bind(&Server::decreaseClientsCounter, this, _1));
    act->start();
    ClientManager::getInstance()->addClient(act);
}

void Server::startServer() {
    start();
}

void Server::stopServer() {
    ClientManager::getInstance()->terminate();
    ControllerManager::getInstance()->terminate();
    terminate();
}

void Server::setControllersStartedFunction(const boost::function<void()> & fn) {
    controllers_started = fn;
}

void Server::setClientsStartedFunction(const boost::function<void()> & fn) {
    clients_started = fn;
}

void Server::started() {
    
}

void Server::stopped() {
    
}

void Server::decreaseControllersCounter(ActiveObjectPtr) {
    getIOService()->dispatch([&]() {
        if(controllers_expected > 0 ){
            controllers_expected--;
            if(controllers_expected == 0) {
                controllers_started();
            }
        }
    });
}

void Server::decreaseClientsCounter(ActiveObjectPtr) {
    getIOService()->dispatch([&]() {
        if(clients_expected > 0 ){
            clients_expected--;
            if(clients_expected == 0) {
                clients_started();
            }
        }
    });
}
