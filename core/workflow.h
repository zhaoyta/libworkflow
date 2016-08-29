#ifndef __WORKFLOW_H_
#define __WORKFLOW_H_

#include <tools/jsonable.h>
#include <tools/defines.h>
#include <boost/uuid/uuid.hpp>
#include <tools/logged.h>
#include <queue>

#pragma GCC visibility push(default)
SHARED_PTR(Workflow);
SHARED_PTR(Timed);
SHARED_PTR(StateMachine);
SHARED_PTR(Controller);
SHARED_PTR(Request);
SHARED_PTR(Session);
class ErrorReport;

struct RunningSession {
    RunningSession();
    virtual ~RunningSession();
    
    //! original request, for tracking purpose
    RequestPtr original;
    //! Well the session, essential.
    SessionPtr session;
    //! Queued requests.
    std::queue<RequestPtr> pendingRequests;
    //! Workflow will control also maximum duration of it's requests.
    TimedPtr timed;
    
    bool active;
};

namespace boost {
    class recursive_mutex;
};

/**
 Whereas StateMachine guaranties execution order, Workflow ensure executability. as such, state machine are exclusively monothreaded.
 They can't handle 2 threads at once. 
 
 (Well, it might actually not be true, I had some alteration done in request id handling, so it might still work. To be tested :p )
 
    Anyway, Workflow is the one to create a new Session if none where provided, and it the one to erase it when it's finished.
    If two requests come for the same session, only the first will be served, the second will be pending up until the first finishes. 
 
    Some request will get ignored, mostly if they relate to interrupt/error/status/reply and their id doesn't match any session. 
    But also if they were pending and the state machine has finished. 
 
    Also, Workflow is responsible for it's request timeout.
 */
class Workflow : public Jsonable, public boost::enable_shared_from_this<Workflow>, public Logged {
    //! Stores session related meta data.
    
    
    StateMachinePtr stateMachine;
    ControllerPtr controller;
    boost::shared_ptr<boost::recursive_mutex> mutex;

    std::string name;
    
    std::map<boost::uuids::uuid, RunningSession> sessions;
    
    double timeout;
    
public:
    Workflow(const std::string &);
    virtual ~Workflow();
    
    //! bind thiw workflow to the controller;
    void setController(ControllerPtr);
    
    const RunningSession & getRunningSession(boost::uuids::uuid & id) const;
    
    //! main entrypoint of a workflow, will check executability and execute the request.
    bool perform(RequestPtr);
    
    //! @return statemachine
    //! allow definition of the process to execute.
    StateMachinePtr getStateMachine();
    
    //! initial configuration ... tell how long a request can last.
    //! default to 60 s.
    void setTimeout(double = 60);
    
    std::string getName() const;

protected:
    //! check whether a request can be executed or not.
    bool canExecuteRequest(RequestPtr);
    
    //! a request as timed out.
    void requestTimedOut(boost::uuids::uuid id);
    
    //! reply with error.
    void errorReply(RequestPtr, ErrorReport *);
    
    bool shouldMakePending(RequestPtr);
    
    void addToPending(RequestPtr);
    
    
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
    
};

OSTREAM_HELPER_DECL(Workflow);

#pragma GCC visibility pop

#endif // __WORKFLOW_H_