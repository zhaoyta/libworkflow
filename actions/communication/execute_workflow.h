#ifndef __EXECUTE_WORKFLOW_H_
#define __EXECUTE_WORKFLOW_H_

#include <core/action.h>
#include <tools/defines.h>

SHARED_PTR(ExecuteWorkflow);
/**
 ExecuteWorkflow will try to execute a workflow. Multiples use cases are expected: 
 
    * It's an existing workflow : Provide a controller and Workflow name.
    * It's a dynamic workflow setup: a property with controller and workflow will have to be filled.
 */
class ExecuteWorkflow: public Action {
    bool prepared;
public:
    //! this constructor is defacto dynamic.
    ExecuteWorkflow();
    //! this constructor is defacto static.
    ExecuteWorkflow(const std::string & controller, const std::string & workflow);
    ~ExecuteWorkflow();
    
    Result perform(SessionPtr) const override;
    
    //! Called when request is async.
    Result replyReceived(SessionPtr, RequestPtr) const override;
    
    //! allow to endure error as if nothing happened. Albeit, the error message will be printed. Otherwise contract shall be met by default values.
    bool canHandleError(SessionPtr) const override;
    
    
    
    //! will check whether provided input is expected or not.
    //! This is a validation check (and not an actual on the spot check )
    ErrorReport expectInput(const std::string &) const override;
    
    //! will check whether provided output is expected or not.
    //! This is a validation check (and not an actual on the spot check )
    ErrorReport expectOutput(const std::string &) const override;
    
    
    bool checkInputs(SessionPtr, ErrorReport &) const override;
    
    //! this will ensure we're expecting what is expected, due to current selection of controller and workflow.
    //! if setup is set to be dynamic, prepare will be called within check inputs.
    //! Otherwise it'll be called on constructor.
    //! Note: this is a bad trick to have it executed within check inputs ...
    //!   And it should be called only once, so long provided / expected workflow doesn't change.
    //! @return false when requested workflow isn't known. 
    bool prepare(SessionPtr);
    
    
};

OSTREAM_HELPER_DECL(ExecuteWorkflow);

#endif // __EXECUTE_WORKFLOW_H_