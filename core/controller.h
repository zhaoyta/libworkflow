#ifndef __CONTROLLER_H_
#define __CONTROLLER_H_

#include <tools/active_object.h>
#include <tools/defines.h>
#include <core/workflow.h>

SHARED_PTR(Controller);
SHARED_PTR(ControllerSpawn);
SHARED_PTR(Workflow);

/**
    Stores workflows, can add some meta data to requests.
    You may add here some scheduling mecanism or request perform control
 */
class Controller: public ActiveObject,
                  public boost::enable_shared_from_this<Controller> {
    std::map<std::string, WorkflowPtr> workflows;
public:
    Controller(const std::string & name, uint32_t pool);
    virtual ~Controller();
    
    //! cast helper :)
    ControllerPtr shared_from_this();
    //! try to find an appropriate workflow to execute this request.
    virtual bool perform(RequestPtr);

    //! by default simply add an empty struct, but can be used to add some meta data.
    //! Note, default add only data to DefaultAction kind of request.
    ControllerSpawnPtr spawnForRequest(RequestPtr);
    
    //! register a workflow in this controller
    void addWorkflow(WorkflowPtr);
};

#endif // __CONTROLLER_H_