#ifndef __HTTP_CLIENT_H_
#define __HTTP_CLIENT_H_

#include <service/client.h>

/**
 This client belong to HTTP Input. 
 When a request has been registered by HTTPInput
 Note: NoReply request are not permitted. Each request will be forced to Reply. (Thus ensure that the connection is released.
 */
class HTTPClient: public Client {
public:
    HTTPClient();
    virtual ~HTTPClient();
};

#endif // __HTTP_CLIENT_H_