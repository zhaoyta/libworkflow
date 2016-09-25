#ifndef __TCP_CLIENT_H_
#define __TCP_CLIENT_H_

#include <service/client.h>

/**
 TCP Client is spawned from TCP Input. It represent an established tcp connection.
 */
class TCPClient: public Client {
public:
    TCPClient();
    virtual ~TCPClient();
};

#endif // __TCP_CLIENT_H_