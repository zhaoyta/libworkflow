#ifndef __SESSION_H_
#define __SESSION_H_

#include <tools/defines.h>
#include <set>
#include <vector>
#include <map>

SHARED_PTR(Session);
SHARED_PTR(PropertySet);
SHARED_PTR(Request);
SHARED_PTR(Context);
SHARED_PTR(ControllerSpawn);
class Target;

BEGIN_ENUM_DECL(ExecutionStatus) {
    Unplanned, //!< Isn't planned for execution
    Pending, //! Is partially clear for execution. Means, it has all required input set, but might get more intel later on.
    Planned, //!< All clear for execution
    Skipped, //!< A skip has been placed on one of it's input, thus can't be executed.
    Waiting, //!< Waiting for Async Request to conclude.
    Async, //!< Waiting for Async Operation to conclude.
    Done, //!< Has been executed successfully.
};
END_ENUM_DECL(ExecutionStatus, Unplanned, "Unplanned");

/**
 Session stores request long execution informations. Mostly used by StateMachine to store it's status, but can be used also by actions to set some internal stuff, like counter, and such. 
 */
class Session {
    RequestPtr request;
    std::vector<RequestPtr> requests;
    PropertySetPtr bypass;
    std::map<int32_t, PropertySetPtr> action_bypasses;
    
    std::set<Target> subqueries;
    std::set<int32_t> pendings;
    std::set<int32_t> nexts;
    std::map<int32_t, EExecutionStatus> status;
    
    std::map<int32_t, std::map<std::string, ContextPtr> > inputs;
    std::map<int32_t, std::map<std::string, ContextPtr> > outputs;
    
    uint32_t current_execution_level;
    
    bool finished;
public:
    Session();
    virtual ~Session();
    
    PropertySetPtr getBypass();
    PropertySetPtr getBypass(int32_t action_id);
    
    RequestPtr getOriginalRequest() const;
    const std::vector<RequestPtr> & getRequests() const;
    RequestPtr getLastRequest() const;
    void pushRequest(RequestPtr);
    
    EExecutionStatus getStatus(int32_t action_id);
    void setStatus(int32_t action_id, EExecutionStatus);
    
    ControllerSpawnPtr getControllerSpawn();
    template<class T>
    boost::shared_ptr<T> getCastedControllerSpawn() {
        return boost::dynamic_pointer_cast<T>(getControllerSpawn());
    }
    
    std::set<int32_t> & getNexts();
    std::set<int32_t> & getPendings();
    std::set<Target>  & getSubQueries();
    std::map<int32_t, EExecutionStatus> & getStatus();
    
    std::map<int32_t, std::map<std::string, ContextPtr> > & getInputs();
    std::map<int32_t, std::map<std::string, ContextPtr> > & getOutputs();
    
    void setOutput(int32_t, const std::string & output, ContextPtr );
    void setInput(int32_t, const std::string & input, ContextPtr);
    
    ContextPtr getOutput( int32_t, const std::string & output);
    
    ContextPtr getInput( int32_t, const std::string & input);
    template<class T>
    boost::shared_ptr<T> getCastedInput(int32_t action_id, const std::string & input) {
        return boost::dynamic_pointer_cast<T>(getInput(action_id, input));
    }
    
    void addSubQuery(const Target & t);
    void removeSubQuery(const Target & t);
    
    uint32_t getCurrentExecutionLevel() const;
    void upCurrentExecutionLevel();
    
    bool hasFinished();
    void setFinished();
};

OSTREAM_HELPER_DECL(Session);

#endif // __SESSION_H_