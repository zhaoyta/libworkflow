#ifndef __SERVER_H_
#define __SERVER_H_

#include <tools/active_object.h>
#include <boost/function.hpp>
#include <tools/defines.h>

SHARED_PTR(Controller);
SHARED_PTR(Actor);

/**
 Server is a simple Actor, it waits for controller and client manager, and default controllers to be ready.
 
 If user provide more controller, it'll wait for them to be started too. 
 As well as Actor, and such.
 
 This is an helper to have things synchronized and ready. It also help to clear things up.
 */
class Server : public ActiveObject {
    boost::function<void()> controllers_started;
    boost::function<void()> clients_started;
    uint32_t controllers_expected;
    uint32_t clients_expected;
    uint32_t default_pool;
    bool clients_done;
    bool controllers_done;
    std::vector<ActorPtr> actors_to_start;
    std::vector<ControllerPtr> controllers_to_add;
public:
    Server();
    virtual ~Server();
    
    //! add a controller to the to start stack.
    //! Note: expect that their start to be delayed.
    void addController(ControllerPtr);
    //! Add an actor to the to start stack.
    //! Note: it's expected that their start to be delayed ...
    void addActor(ActorPtr);
    
    //! set number of thread avaible for default and temporary controller.
    void setDefaultControllersPool(uint32_t);
    
    //! this will trigger Server start, starting controllers manager and clients managers.
    void startServer();
    //! This will stop client managers and controller managers then quit everything
    void stopServer();
    
    //! this allow a callback that will be triggered when all controllers have started.
    void setControllersStartedFunction(const boost::function<void()> & );
    //! This allow a callback that will be triggered when all actors have been started.
    void setClientsStartedFunction(const boost::function<void()> & );
    
protected:
    //! when all controllers have been started will trigger controllers started function, default post controller actions, and post controller actions.
    void decreaseControllersCounter(ActiveObjectPtr);
    
    //! when all clients and controllers have been started will trigger clients started function, default post clients actions, and post clients actions.
    void decreaseClientsCounter(ActiveObjectPtr);
    
    //! register default workflow, fetch references among others.
    void defaultPostControllersActions();
    void defaultPostClientsActions();
    
    virtual void postControllersActions();
    virtual void postClientsActions();
    
    void controllersManagerStarted(ActiveObjectPtr);
    void clientsManagerStarted(ActiveObjectPtr);
};

#endif // __SERVER_H_