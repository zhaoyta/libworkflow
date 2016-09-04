#ifndef __ACTOR_H_
#define __ACTOR_H_

#include <service/client.h>
#include <tools/active_object.h>
#include <tools/defines.h>
#include <queue>

SHARED_PTR(Actor);
SHARED_PTR(Request);
class ErrorReport;

/**
    An Actor is a Client that lives. As a client it can send an receive requests.
 As an Actor it has access to it's own thread.
 */
class Actor: public Client, public ActiveObject {
    std::queue<RequestPtr> pending_requests;
public:
    Actor(const std::string & name);
    virtual ~Actor();
    
    ActorPtr shared_from_this();
    
    //! This is for logging :)
    std::string logActor() const;
    
    //! kill the actors thread and set it to disconnected.
    void disconnect() override;
    
    //! attempt to restart actor :)
    void connect() override;
    
    
    //! Tell whether there is a request awaiting your attention.
    bool hasPendingRequest() const;
    //! Grab next request in line.
    RequestPtr dequeuePendingRequest();
    //! This will simply get an idea of what is comming next ;)
    RequestPtr peekNextRequest();
protected:
    
    //! this is some sugar to send request to ControllerManager.
    void publishRequest(RequestPtr);
    //! Same, except that it'll reply with error :)
    void replyError(RequestPtr, const ErrorReport & );
    
    // dont forget you've got access to :
    // IOServicePtr getIOService(); //< Which is your way to the thread.
    
    //! This allow to handle request
    //! (by Sending it means actually pushing it to pendingrequests.
    bool canSendRequest(RequestPtr, ErrorReport &) override;
    //! this is the actual pushing to pending requests.
    bool doSendRequest(RequestPtr) override;
    
    //! This is called when a request has been received.
    //! Note, it is possible that no request are pending when you get this callback ;)
    //! As you maight handle multiple request with only one call
    //! with multiple call to dequeuePendingRequest().
    virtual void newRequestReceived();
    

    void started() override;
    void stopped() override;
    
    
};

#endif // __ACTOR_H_