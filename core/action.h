#ifndef __ACTION_H_
#define __ACTION_H_


#pragma GCC visibility push(default)

SHARED_PTR(StateMachine);
SHARED_PTR(Session);
SHARED_PTR(PropertySet);
SHARED_PTR(Context);

class PutDefinition;
class ActionMetaPriv;
class Result;
class ErrorReport;

/**
 Action is where the business code go. 
 Concidere it totally concurrent, as such, it musn't hold any non const property.
 If you want to store temporary data, use it's own PropertySet, or the one in the Session.
 
 Note, when declaring an Action you're in fact declaring a contract. 
 The action will ensure this contract is met.
 
 */
class Action {
    StateMachinePtr state_machine;
    int32_t action_id;
    std::string name;
    
    //!  Internal MetaData
    ActionMetaPriv * pmeta;
    
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
    const std::string & getName() const;
    void setName(const std::string & name);
    
    int32_t getActionId() const;
    
    PropertySetPtr properties();
    
protected:
    //! no need to expose that one ;)
    StateMachinePtr getStateMachine() const;

    //! Results
    
    //! Tells state machine that everything has been done here, successfully.
    Result done() const;
    //! Tells state machine to expect a call from outside, it need to be redirected here.
    Result wait() const;
    //! Tells state machine that WE are not finished yet, but it should expect a call from us anytime.
    Result async() const;
    //! Tells state machine that something failed. Provided @a error_report will provide informations.
    Result error(const ErrorReport &) const;
    //! When in async mode, this contacts the state machine and tell we're done.
    void asyncDone() const;
    //! async counterpart of wait()
    void asyncWait() const;
    //! async counterpart of error()
    void asyncError(const ErrorReport &) const;
    
    
    //! For loggin, provide informations on which Workflow is running, and what request is being executed
    std::string fingerprint(SessionPtr) const;
    
    //! properties accessors
    //! @sa PropertySet
    //! Properties can be accessed from multiples sources, prioritized as follow
    //! Request > Session > Action
    
    //! Fetch a double value for @a key
    double doubleValue(SessionPtr, const std::string & key, double default = 0.) const;
    //! Fetch a bool value for @a key
    bool boolValue(SessionPtr, const std::string & key, bool default = false) const;
    //! Fetch a string value for @a key
    std::string stringValue(SessionPtr, const std::string & key, const std::string & default = "") const;
    //! Fetch a uint value for @a key
    uint32_t uintValue(SessionPtr, const std::string & key, uint32_t default = 0) const;
    //! Fetch a Context
    ContextPtr customValue(SessionPtr, const std::string & key, ContextPtr default = ContextPtr()) const;
    template<class T>
    boost::shared_ptr<T> customCastedValue(SessionPtr session, const std::string & key, ContextPtr default= ContextPtr()) {
        return boost::dynamic_pointer_cast<T>(customValue(session,key,default));
    }
    
    
    //! Input / Output definition.
    void defineInput(const std::string & name, TypeChecker * , bool mandatory = true);
    void defineInput(const PutDefinition & );
    
    void defineOutput(const std::string & name, TypeChecker * , bool mandatory = true);
    void defineOutput(const PutDefinition &);
    
    //! Input / Output accessor
    
    ContextPtr getInput(SessionPtr, const std::string & name);
    template<class T>
    boost::shared_ptr<T> getCastedInput(SessionPtr session, const std::string & key) {
        return boost::dynamic_pointer_cast<T>(getInput(session, key));
    }
    
    void setOutput(SessionPtr, const std::string & name, ContextPtr);
    
    const std::set<PutDefinition> & getInputs() const;
    const std::set<PutDefinition> & getOutputs() const;
    
    void clearInputs();
    void clearOutputs();
    
};

OSTREAM_HELPER_DECL(Action);


#pragma GCC visibility pop
#endinf // __ACTION_H_