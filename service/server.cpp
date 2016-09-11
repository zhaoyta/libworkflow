#include <service/server.h>
#include <service/controller_manager.h>
#include <service/client_manager.h>
#include <core/controller.h>
#include <service/actor.h>

Server::Server() : ActiveObject("Server", false), expected(0) {
    
}

Server::~Server() {
    
}

void Server::addController(ControllerPtr ctr) {
    expected++;
    ctr->setStartedFunction(boost::bind(&Server::decreaseCounter, this));
    ctr->start();
    ControllerManager::getInstance()->registerController(ctr);
}

void Server::addActor(ActorPtr act) {
    expected++;
    act->setStartedFunction(boost::bind(&Server::decreaseCounter, this));
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

void Server::setServerStartedFunction(const boost::function<void()> & fn) {
    server_started = fn;
}

void Server::started() {
    
}

void Server::stopped() {
    
}

void Server::decreaseCounter(ActiveObjectPtr) {
    getIOService()->dispatch([&]() {
        if(expected > 0 ){
            expected--;
            if(expected == 0) {
                server_started();
            }
        }
    });
}

