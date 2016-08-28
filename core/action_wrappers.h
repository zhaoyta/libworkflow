#ifndef __ACTION_WRAPPERS_H_
#define __ACTION_WRAPPERS_H_

#include <core/action.h>
#include <tools/defines.h>

class Result;
SHARED_PTR(ActionWrapper);

/**
 Standard Wrapper, will simply execute provided action, name, and other stuff are inherited from provided actions.
 */
class ActionWrapper : public Action {
    ActionPtr wrapped;
public:
    ActionWrapper( ActionPtr );
    virtual ~ActionWrapper();
    
    
    bool checkInputs(SessionPtr, ErrorReport &) const override;
    bool checkOutputs(SessionPtr, ErrorReport &) const override;
    Result perform(SessionPtr) const override final;
    Result replyReceived(SessionPtr, RequestPtr) const override final;
    bool canPerform(SessionPtr, ErrorReport &) const override;
    bool canHandleError(SessionPtr) const override;
    void setActionId(int32_t action_id) override;
    int32_t getActionId() const override;
    
protected:
    //! This get called whenever a perform is requested.
    virtual void wrapPerform(SessionPtr) const;
    //! This get called after a perform is requested.
    virtual void wrapPostPerform(SessionPtr) const;
    //! This get called whenever a reply received is requested.
    virtual void wrapReplyReceived(SessionPtr, RequestPtr) const;
    //! This get called after a reply received is requested.
    virtual void wrapPostReplyReceived(SessionPtr, RequestPtr) const;
};

/** 
 This will ensure that all pending request are aborted.
 */
class InterruptWrapper : public ActionWrapper {
public:
    InterruptWrapper(ActionPtr);
    
protected:
    void wrapPerform(SessionPtr) const override;
};

/**
 This will ensure that all pending request are aborted.
 Gather contexts linked to finish action, and send them
 */
class FinishWrapper : public InterruptWrapper {
public:
    FinishWrapper(ActionPtr);
    
protected:
    void wrapPerform(SessionPtr) const override;
};

/**
 This will ensure that all pending request are aborted.
 Record provided error and reply to caller.
 */
class ErrorWrapper : public InterruptWrapper {
public:
    ErrorWrapper(ActionPtr);
    
protected:
    void wrapPerform(SessionPtr) const override;
};

/**
 This will ensure that all pending request are aborted.
 Does nothing atm :)
 */
class CleanupWrapper : public InterruptWrapper {
public:
    CleanupWrapper(ActionPtr);
    
protected:
    void wrapPerform(SessionPtr) const override;
};
#endif // __ACTION_WRAPPERS_H_