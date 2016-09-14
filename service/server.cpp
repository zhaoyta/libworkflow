#include <service/server.h>
#include <service/controller_manager.h>
#include <service/client_manager.h>
#include <core/controller.h>
#include <service/actor.h>
#include <boost/bind.hpp>
#include <core/state_machine.h>
#include <core/bindings.h>
#include <actions/tools/fetch_refs.h>
#include <tools/action_factory.h>
#include <core/action.h>

Server::Server() : ActiveObject("Server", 1,false), controllers_expected(0), clients_expected(0), default_pool(5), clients_done(false), controllers_done(false) {
    
}

Server::~Server() {
    
}

void Server::addController(ControllerPtr ctr) {
    controllers_expected++;
    ctr->setStartedFunction(boost::bind(&Server::decreaseControllersCounter, this, _1));
    controllers_to_add.push_back(ctr);
}

void Server::addActor(ActorPtr act) {
    clients_expected++;
    act->setStartedFunction(boost::bind(&Server::decreaseClientsCounter, this, _1));
    actors_to_start.push_back(act);
}

void Server::startServer() {
    
    auto controller = ControllerPtr(new Controller("default", default_pool, true));
    addController(controller);
    controller = ControllerPtr(new TemporaryController(default_pool, true));
    addController(controller);
    
    // add here server mandatory actors ... there shoudln't be many of them, but well it could happend :) a RequestScheduler could be added, or some stuff like that :)
    ControllerManager::getInstance(true)->setStartedFunction(boost::bind(&Server::controllersManagerStarted, this, _1));
    ClientManager::getInstance(true)->setStartedFunction(boost::bind(&Server::clientsManagerStarted, this, _1));
    ControllerManager::getInstance()->start();
    start();
}

void Server::controllersManagerStarted(ActiveObjectPtr) {
    ClientManager::getInstance()->start();
    auto instance= ControllerManager::getInstance();
    for(const auto & ctr: controllers_to_add) {
        ctr->start();
        instance->registerController(ctr);
    }
}

void Server::clientsManagerStarted(ActiveObjectPtr) {
    auto instance= ClientManager::getInstance();
    for(const auto & act: actors_to_start) {
        act->start();
        instance->addClient(act);
    }
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

void Server::decreaseControllersCounter(ActiveObjectPtr) {
    getIOService()->dispatch([&]() {
        if(controllers_expected > 0 ){
            controllers_expected--;
            if(controllers_expected == 0) {
                defaultPostControllersActions();
                postControllersActions();
                if(controllers_started)
                    controllers_started();
                controllers_done = true;
            }
            if(clients_expected == 0 and not clients_done) {
                defaultPostClientsActions();
                postClientsActions();
                if(clients_started)
                    clients_started();
                clients_done = true;
            }
        }
    });
}

void Server::decreaseClientsCounter(ActiveObjectPtr) {
    getIOService()->dispatch([&]() {
        if(clients_expected > 0 ){
            clients_expected--;
            if(clients_expected == 0 and controllers_done and not clients_done) {
                defaultPostClientsActions();
                postClientsActions();
                if(clients_started)
                    clients_started();
                clients_done = true;
            }
        }
    });
}

void Server::setDefaultControllersPool(uint32_t pool) {
    default_pool = pool;
}

void Server::defaultPostControllersActions() {
    BOOST_LOG_SEV(logger, Info) << " Server: Adding default workflows ...";
    
    WorkflowPtr workflow(new Workflow("FetchReferences"));
    auto sm = workflow->getStateMachine();
    sm->addAction(0, new FetchRefs(), {
        OutputBinding("refs", Step::Finish, "refs")
    });
    sm->addInput(InputBinding("", 0, ""));
    ControllerManager::getInstance()->getController()->addWorkflow(workflow);
    
    ActionFactory::registerAction(new ActionBuilder<DefaultNextAction>());
}

void Server::defaultPostClientsActions() {
}

void Server::postControllersActions() {
    
}

void Server::postClientsActions() {
    
}
