#ifndef __STATE_MACHINE_H_
#define __STATE_MACHINE_H_

#include <tools/defines.h>
#include <tools/jsonable.h>
#include <map>
#include <set>
#include <vector>
#include <core/steps.h>
#include <tools/logged.h>
#include <boost/uuid/uuid.hpp>


#pragma GCC visibility push(default)
SHARED_PTR(StateMachine);
SHARED_PTR(Request);
SHARED_PTR(Session);
SHARED_PTR(Action);
SHARED_PTR(Binding);
SHARED_PTR(Workflow);

class InputBinding;
class OutputBinding;
class Result;
class ErrorReport;

/**
 StateMachine define how actions are interlinked, and define which action is running and what actions are expected to run.
 */
class StateMachine : public Jsonable , public Logged, public boost::enable_shared_from_this<StateMachine> {
    WorkflowWPtr workflow;
    
    std::map<int32_t, ActionPtr> actions;
    std::map<int32_t, std::vector<OutputBinding> > outputs;
    std::map<int32_t, std::set<int32_t> > inputs_map;
    std::vector<InputBinding> starters;
    std::string workflow_name;
public:
    StateMachine();
    virtual ~StateMachine();
    
    //! Set default layout of a workflow.
    void init();
    
    //! begin execution of request. will sort out if its a new one or not, and dispatch request to appropriate handler.
    virtual bool execute(SessionPtr, RequestPtr) ;    
    
    //! this is a configuration method, will store action in this state machine, and connect it to other actions.
    void addAction(Step step, Action *, const std::vector<OutputBinding> & );
    void addAction(Step step, ActionPtr, const std::vector<OutputBinding> & );
    void addAction(int32_t action_id, Action *, const std::vector<OutputBinding> & );
    virtual void addAction(int32_t action_id, ActionPtr, const std::vector<OutputBinding> & );
    //! set initial actions of this state machine.
    virtual void addInput(const InputBinding & );
    
    //! thats all actions.
    const std::map<int32_t, ActionPtr> getActions() const;
    
    //! tell whether this session has finished it's job.
    bool finished(SessionPtr);
    
    //! This is called by action in async mode.
    void actionAsyncFinished(SessionPtr, const Result &);
    
    //! retrieve the workflow
    WorkflowWPtr getWorkflow();
    void setWorkflow(WorkflowWPtr);
    
    
    std::vector<std::string> getExpectedOutput() const;
    
    //! for logging purpose
    std::string fingerprint(SessionPtr);
    
protected:
    
    //! first call  :) start everything.
    bool firstCall(SessionPtr) ;
    //! this handler is called when an interrupt has be received. will up current execution level and execute interrupt action.
    bool interruptReceived(SessionPtr) ;
    //! this handler is called upon receiving an error and execute error action.
    bool errorReceived(SessionPtr, RequestPtr) ;
    //! this function is called upon receiving a status request, it should be overriden to have any effect ( atm reply with an empty reply )
    bool statusReceived(SessionPtr) ;
    //! this handler is called when a reply has been received, check whether it was expected or not.
    bool replyReceived(SessionPtr, RequestPtr);
    
    //! this function does all check prior executing the action.
    bool executeAction(SessionPtr, int32_t action) ;
    
    //! this will check what result the action provided, and declare what comes next.
    void actionExecuted(SessionPtr, const Result &) ;
    
    //! @return -1 if none where found. Otherwise seek next in line.
    //! Note this removes it from nexts.
    int32_t getNext(SessionPtr);
    //! @return -1 if none where found. Otherwise seek next in line.
    //! Note this removes it from pending.
    int32_t getNextPending(SessionPtr);
    
    //! add action to nexts to be executed.
    void addToNext(SessionPtr, int32_t);
    //! add action to pendings execution ( means, they could get executed, but has unfilled inputs that may comes later.
    void addToPending(SessionPtr, int32_t);
    
    //! remove from nexts.
    void removeFromNext(SessionPtr, int32_t);
    //! remove from pending
    void removeFromPending(SessionPtr, int32_t);
    
    //! Used in case of no next nor pending available.
    //! will check if sufficient conditions are met for finish to get executed.(means at least one guy hit finish.)
    bool canExecuteFinish(SessionPtr);
    
    //! Check whether this action can be exectued.
    bool canExecuteAction(SessionPtr, ActionPtr, ErrorReport &);
    //! check whether this action can be set to pending.
    bool canPendAction(SessionPtr, ActionPtr);
    
    //! will transpose outputs of given action to inputs of its bound actions.
    //! will also add items to pendings and nexts.
    void bindResults(SessionPtr, int32_t action_id);
    
    //! Check if we're able to promote a pending to Execution.
    //! @return true if one has been found.
    bool tryPromotePending(SessionPtr);
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
    
};

OSTREAM_HELPER_DECL(StateMachine);


#pragma GCC visibility pop

#endif //__STATE_MACHINE_H_