#ifndef __CLIENT_MANAGER_H_
#define __CLIENT_MANAGER_H_

#include <tools/active_object.h>
#include <tools/logged.h>
#include <map>
#include <boost/uuid/uuid.hpp>
#include <tools/defines.h>

SHARED_PTR(Client);
SHARED_PTR(ClientManager);
SHARED_PTR(Timed);

/**
    ClientManager is a simple storage, that is responsible for periodic check of client availability. 
    It's also able to idenditify and recover a Client by it's ID.
 
    Note this is a basic Active Object, thus
    It's expected that public action can be executed anywhere
    Protected should be called only within it's thread.
 */
class ClientManager : public ActiveObject {
    std::map<boost::uuids::uuid, ClientPtr> clients;
    TimedPtr timer;
    ClientManager();
public:
    virtual ~ClientManager();
    //! Singleton :)
    static ClientManagerPtr getInstance();
    
    //! Grab a client by it's ID.
    ClientPtr getClient(const boost::uuids::uuid & cid);
    //! Add a client to the stack.
    void addClient(ClientPtr);
    
    
    void checkClients();
    
protected:
    //! Called by Timer, mostly. check clients availability.
    void checkClients(const boost::system::error_code &);
    
    //! drop a client.
    void removeClient(const boost::uuids::uuid & cid);

protected:
    void started();
    
};

OSTREAM_HELPER_DECL(ClientManager);

#endif // __CLIENT_MANAGER_H_