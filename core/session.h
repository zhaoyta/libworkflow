#ifndef __SESSION_H_
#define __SESSION_H_

#include <tools/defines.h>

SHARED_PTR(Session);
SHARED_PTR(PropertySet);

/**
 Session stores request long execution informations. Mostly used by StateMachine to store it's status, but can be used also by actions to set some internal stuff, like counter, and such. 
 */
class Session {
    PropertySetPtr bypass;
    std::map<int32_t, PropertySetPtr> action_bypasses;
public:
    Session();
    virtual ~Session();
    
    
    
};

OSTREAM_HELPER_DECL(Session);

#endif // __SESSION_H_