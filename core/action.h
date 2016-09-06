#ifndef __ACTION_H_
#define __ACTION_H_


#include <tools/defines.h>
#include <set>

//! added for convenience ...

#include <core/session.h>
#include <core/request.h>
#include <core/context.h>
#include <core/result.h>
#include <tools/logged.h>
#include <tools/property_set.h>
#include <tools/type_checker.h>

#pragma GCC visibility push(default)

SHARED_PTR(StateMachine);
SHARED_PTR(Session);
SHARED_PTR(PropertySet);
SHARED_PTR(Context);
SHARED_PTR(Request);

class PutDefinition;
class Result;
class ErrorReport;
class TypeChecker;

/**
 Action is where the business code go. 
 Concidere it totally concurrent, as such, it musn't hold any non const property.
 If you want to store temporary data, use it's own PropertySet, or the one in the Session.
 
 Note, when declaring an Action you're in fact declaring a contract. 
 The action will ensure this contract is met.
 
 */
class Action : public Logged, public Jsonable {
    StateMachineWPtr state_machine;
    int32_t action_id;
    std::string name;
    
    PropertySetPtr propertyset;
    
    std::set<PutDefinition> inputs;
    std::set<PutDefinition> outputs;
    

public:
    Action(const std::string & name);
    virtual ~Action();
    
    //! Does the actual input check. Ensure that all input provided to this action match the one expected.
    //! Or are allowed to be executed non the less.
    virtual bool checkInputs(SessionPtr, ErrorReport &) const;
    //! Same with output.
    virtual bool checkOutputs(SessionPtr, ErrorReport &) const;
    
    //! this is the main method of Action, this is where you'll write your business code.
    //! Result can be generated using error(), done(), wait() or async() functions.
    virtual Result perform(SessionPtr) const;
    
    //! This is called after a wait(), with the request that triggered it.
    virtual Result replyReceived(SessionPtr, RequestPtr) const;
    
    //! This is a non input related check. will always be called before execution.
    //! @return true by default, if set to false, it's expected @error_report to be set.
    virtual bool canPerform(SessionPtr, ErrorReport &) const;
    
    virtual bool canHandleError(SessionPtr) const;
    
    //! some accessors ...
    virtual const std::string & getName() const;
    virtual void setName(const std::string & name);
    
    virtual void setActionId(int32_t action_id);
    virtual int32_t getActionId() const;
    
    virtual PropertySetPtr properties();
    
    std::string logAction() const;
    
    
    void setStateMachine(StateMachineWPtr);
    StateMachineWPtr getStateMachine() const;
    
protected:
    

    //! Results
    
    //! Tells state machine that everything has been done here, successfully.
    Result done() const;
    //! Tells state machine to expect a call from outside, it need to be redirected here.
    Result wait() const;
    //! Tells state machine that WE are not finished yet, but it should expect a call from us anytime.
    Result async() const;
    //! Tells state machine that something failed.
    Result error(SessionPtr, const std::string & err_key, const std::string & error_message) const;
    //! When in async mode, this contacts the state machine and tell we're done.
    void asyncDone(SessionPtr) const;
    //! async counterpart of wait()
    void asyncWait(SessionPtr) const;
    //! async counterpart of error()
    void asyncError(SessionPtr, const std::string & err_key, const std::string & err_message) const;
    
    //! send request to be executed.
    Result executeSyncRequest(SessionPtr, RequestPtr) const;
    //! ensure the request knows how to come back here.
    //! If none is provided, will simply create a new one.
    RequestPtr prepareSyncRequest(SessionPtr, RequestPtr = RequestPtr()) const;
    //! will execute this request and dont expect it to reply anytime.
    void executeAsyncRequest(RequestPtr) const;
    
    //! For loggin, provide informations on which Workflow is running, and what request is being executed
    std::string fingerprint(SessionPtr) const;
    
    //! properties accessors
    //! @sa PropertySet
    //! Properties can be accessed from multiples sources, prioritized as follow
    //! Session (byAction) > Session > Action
    
    //! Fetch a double value for @a key
    double doubleProperty(SessionPtr, const std::string & key, double def = 0.0) const;
    //! Fetch a bool value for @a key
    bool boolProperty(SessionPtr, const std::string & key, bool def = false) const;
    //! Fetch a string value for @a key
    std::string stringProperty(SessionPtr, const std::string & key, const std::string & def = "") const;
    //! Fetch a uint value for @a key
    uint32_t uintProperty(SessionPtr, const std::string & key, uint32_t def = 0) const;
    //! Fetch a Context
    ContextPtr customProperty(SessionPtr, const std::string & key, ContextPtr def = ContextPtr()) const;
    template<class T>
    boost::shared_ptr<T> customCastedProperty(SessionPtr session, const std::string & key, ContextPtr def= ContextPtr()) {
        return boost::dynamic_pointer_cast<T>(customProperty(session,key,def));
    }
    
    
    //! Input / Output definition.
    void defineInput(const std::string & name, TypeChecker * , bool mandatory = true);
    void defineInput(const PutDefinition & );
    
    void defineOutput(const std::string & name, TypeChecker * , bool mandatory = true);
    void defineOutput(const PutDefinition &);
    
    //! Input / Output accessor
    
    ContextPtr getInput(SessionPtr, const std::string & name) const;
    template<class T>
    boost::shared_ptr<T> getCastedInput(SessionPtr session, const std::string & key)  const{
        return boost::dynamic_pointer_cast<T>(getInput(session, key));
    }
    
    void setOutput(SessionPtr, const std::string & name, ContextPtr) const;
    void setOutput(SessionPtr, const std::string & name, Context*) const;
    
    const std::set<PutDefinition> & getInputs() const;
    const std::set<PutDefinition> & getOutputs() const;
    
    void clearInputs();
    void clearOutputs();
    
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
private:
    
    //! This should only be called by StateMachine ..;
    void setActionId(int32_t action_id) const;
    friend class StateMachine;
};

OSTREAM_HELPER_DECL(Action);

class DefaultNextAction: public Action {
public:
    DefaultNextAction();
    virtual ~DefaultNextAction();
    
    Result perform(SessionPtr) const override;
};


#pragma GCC visibility pop
#endif // __ACTION_H_