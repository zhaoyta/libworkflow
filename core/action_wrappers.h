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
    virtual void wrapPerform(SessionPtr);
    //! This get called after a perform is requested.
    virtual void wrapPostPerform(SessionPtr);
    //! This get called whenever a reply received is requested.
    virtual void wrapReplyReceived(SessionPtr, RequestPtr);
    //! This get called after a reply received is requested.
    virtual void wrapPostReplyReceived(SessionPtr, RequestPtr);
};

/** 
 This will ensure that all pending request are aborted.
 */
class InterruptWrapper : public ActionWrapper {
public:
    InterruptWrapper(ActionPtr);
    
protected:
    virtual void wrapPerform(SessionPtr);
};

/**
 This will ensure that all pending request are aborted.
 */
class FinishWrapper : public InterruptWrapper {
public:
    FinishWrapper(ActionPtr);
    
protected:
    virtual void wrapPerform(SessionPtr);
};

/**
 This will ensure that all pending request are aborted.
 */
class ErrorWrapper : public InterruptWrapper {
public:
    ErrorWrapper(ActionPtr);
    
protected:
    virtual void wrapPerform(SessionPtr);
};

/**
 This will ensure that all pending request are aborted.
 */
class CleanupWrapper : public ActionWrapper {
public:
    CleanupWrapper(ActionPtr);
    
protected:
    virtual void wrapPerform(SessionPtr);
};
#endif // __ACTION_WRAPPERS_H_