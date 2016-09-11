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
public:
    Server();
    virtual ~Server();
    
    void addController(ControllerPtr);
    void addActor(ActorPtr);
    
    void startServer();
    void stopServer();
    
    void setControllersStartedFunction(const boost::function<void()> & );
    void setClientsStartedFunction(const boost::function<void()> & );
    
protected:
    void started() override;
    void stopped() override;
    
    void decreaseControllersCounter(ActiveObjectPtr);
    void decreaseClientsCounter(ActiveObjectPtr);
};

#endif // __SERVER_H_