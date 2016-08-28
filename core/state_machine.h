#ifndef __STATE_MACHINE_H_
#define __STATE_MACHINE_H_

#include <tools/defines.h>
#include <tools/jsonable.h>
#include <map>
#include <set>
#include <vector>
#include <boost/uuid/uuid.hpp>

SHARED_PTR(StateMachine);
SHARED_PTR(Request);
SHARED_PTR(Session);
SHARED_PTR(Action);
SHARED_PTR(Binding);
SHARED_PTR(Workflow);

class InputBinding;
class OutputBinding;
class Result;

/**
 StateMachine define how actions are interlinked, and define which action is running and what actions are expected to run.
 */
class StateMachine : public Jsonable {
    WorkflowPtr workflow;
    
    std::map<int32_t, ActionPtr> actions;
    std::map<int32_t, std::vector<OutputBinding> > outputs;
    std::map<int32_t, std::set<int32_t> > inputs_map;
    std::vector<InputBinding> starters;
public:
    StateMachine(WorkflowPtr);
    virtual ~StateMachine();
    
    //! begin execution of request. will sort out if its a new one or not, and dispatch request to appropriate handler.
    virtual bool execute(SessionPtr, RequestPtr) ;
    
    //! this is a configuration method, will store action in this state machine, and connect it to other actions.
    virtual void addAction(int32_t action_id, ActionPtr, const std::vector<OutputBinding> & );
    //! set initial actions of this state machine.
    virtual void addInput(const InputBinding & );
    
    //! thats all actions.
    const std::map<int32_t, ActionPtr> getActions() const;
    
    //! tell whether this session has finished it's job.
    bool finished(SessionPtr);
    
    //! This is called by action in async mode.
    void actionAsyncFinished(const Result &);
    
    //! retrieve the workflow
    WorkflowPtr getWorkflow();
    
protected:
    
    //! first call  :) start everything.
    bool firstCall(SessionPtr) ;
    //! this handler is called when an interrupt has be received. will up current execution level and execute interrupt action.
    bool interruptReceived(SessionPtr) ;
    //! this handler is called upon receiving an error and execute error action.
    bool errorReceived(SessionPtr) ;
    //! this function is called upon receiving a status request, it should be overriden to have any effect ( atm reply with an empty reply )
    virtual bool statusReceived(SessionPtr, RequestPtr) ;
    //! this handler is called when a reply has been received, check whether it was expected or not.
    bool replyReceived(SessionPtr, RequestPtr);
    
    //! this function does all check prior executing the action.
    void execute(SessionPtr, int32_t action) ;
    
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
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
    
};

OSTREAM_HELPER_DECL(StateMachine);

#endif //__STATE_MACHINE_H_