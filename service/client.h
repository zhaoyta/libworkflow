#ifndef __CLIENT_H_
#define __CLIENT_H_

#include <tools/defines.h>
#include <tools/logged.h>
#include <boost/uuid/uuid.hpp>
#include <map>

SHARED_PTR(Client);
SHARED_PTR(Request);

class ErrorReport;

BEGIN_ENUM_DECL(ClientStatus) {
    Connected,
    HeartbeatExpected,
    Disconnected,
};
END_ENUM_DECL(ClientStatus, Disconnected, "Disconnected");

/**
 Client is a simple mailbox, that can receive and emit requests.
 Well they're a tad more complicated than this ;)
 
 They get to know what request they emitted outside and which request they sent processing here.
 When a client get disconnected, it automagically interrupt all requests.
 */
class Client : public Logged {
    EClientStatus status;
    boost::uuids::uuid client_id;
    std::map<boost::uuids::uuid, RequestPtr> receivedRequests;
    std::map<boost::uuids::uuid, RequestPtr> sentRequests;
    
public:
    Client();
    virtual ~Client();
    
    //! get it's id.
    boost::uuids::uuid getId() const;
    
    //! nice log display :)
    std::string logClient() const;
    
    //! retrieve client connection status.
    EClientStatus getStatus() const;
    //! set client status, if status changed, will trigger statusChanged()
    //! @param force if set to true, won't call status changed.
    void setStatus(EClientStatus, bool force = false);
    
    //! @return false by default (as it's unconnected)
    //! Tell whether this client need to check it's connected state.
    virtual bool needHeartBeat() const;
    
    //! do the heartbeat. By default does nothing.
    //! note: if heartbeat state is async, please set status to heartbeating.
    //! and it's expected that when the heartbeat replied, you set the status back to Connected.
    virtual void heartbeat();
    
    //! tell whether the heartbeat has timed out.
    //! if it has timed out, client manager will do something drastic
    virtual bool heartbeatTimedOut();
    
    //! Upon heartbeat timeout, either the client manager removes it or not.
    virtual bool shouldDeleteClient();
    
    //! well it does nothing as is, aside setting client to connected state.
    virtual void connect();
    //! doesn't do much as is, expect setting client to disconnected state.
    //! note: this is called when heartbeat timed out.
    virtual void disconnect();
    
    // now comes the fun, Client is a border guy, what comes in and out is somewhat tricky and depend on user point of view.
    
    // by default we expect that a client is an interface with some outbout application.
    // thus, we receive new request from this outside application and we send them.
    
    //! will try to add this request as received.
    bool receiveNewRequest(RequestPtr, ErrorReport &);
    
    //! will try to add this request as sent.
    bool sendRequest(RequestPtr, ErrorReport & );
    
    //! we're processing request from host ( outside )
    bool hostExpectsReply() const;
    //! we're waiting for replies from this host
    bool repliesExpectedFromHost() const;
    
protected:
    //! called when status changes ... default does nothing.
    virtual void statusChanged(EClientStatus from, EClientStatus to);
    
    //! check ability to handle this request.
    //! @return true by default
    virtual bool isReceivedRequestAllowed(RequestPtr, ErrorReport &);
    
    //! check ability to send this request
    //! @return false by default.
    virtual bool canSendRequest(RequestPtr, ErrorReport &);
    
    //! do the actual operation of receiving
    //! This will actually transfert the request to ControllerManager.
    virtual void doReceiveRequest(RequestPtr);
    //! do the actual sending. Does nothing by default
    virtual void doSendRequest(RequestPtr);
    
private:
    //! this does internal magick ...
    //! Ensure that requests from outside get aborted if client get down.
    //! Ensure that request we're expecting data from are aborted if client get down.
    void internalStatusChanged(EClientStatus from, EClientStatus to);
    
};

OSTREAM_HELPER_DECL(Client);

#endif // __CLIENT_H_