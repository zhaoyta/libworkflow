#ifndef __CONTROLLER_MANAGER_H_
#define __CONTROLLER_MANAGER_H_

#include <tools/defines.h>
#include <tools/active_object.h>
#include <map>

SHARED_PTR(ControllerManager);
SHARED_PTR(Controller);
SHARED_PTR(Request);

/**
 ControllerManager is the starting point for everything Request.
 It'll check request for which controller they seek and redirect it to the appropriate one. 
 */
class ControllerManager : public ActiveObject {    
    std::map<std::string, ControllerPtr> controllers;
    uint32_t default_pool;
public:
    ControllerManager(uint32_t default_pool = 10);
    virtual ~ControllerManager();
    
    static ControllerManagerPtr getInstance();
    
    void perform(RequestPtr);
    
    void registerController(ControllerPtr);
    ControllerPtr getController(const std::string & = "default");
protected:
    
    void started() override;
    void stopped() override;
};

OSTREAM_HELPER_DECL(ControllerManager);


#endif // __CONTROLLER_MANAGER_H_