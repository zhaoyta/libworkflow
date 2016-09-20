#ifndef __REQUEST_H_
#define __REQUEST_H_

#include <core/target.h>
#include <tools/jsonable.h>
#include <tools/defines.h>
#include <boost/uuid/uuid.hpp>
#include <map>
#include <tools/error_report.h>


SHARED_PTR(Context);
SHARED_PTR(GroupedCtx);
SHARED_PTR(ControllerSpawn);
SHARED_PTR(PropertySet);
#pragma GCC visibility push(default)
SHARED_PTR(Request);


/**
    Request tells what is to be executed, whom asks it.
 It provide also initial data to be used on workflow start up.
 It allows as well to bypass generally and action specifics properties. 
 
 Note: Bypasses will only be ported for the first call. all subsequent call wont transfert bypasses. ( by subsequent I mean, interrupt/error or callbacks.
 */
class Request: public Jsonable {
    //! What controller/ workflow/ etc is targeted for execution
    Target target;
    //! What target to reply to when finished.
    Target reply;
    //! This might differe from target id as target id might refer to a sub request id. Means, this request_id is the one the client set. It's the reference for logs and such. 
    boost::uuids::uuid request_id;
    //! this is initial (or callbacked) informations to be pushed to the workflow.
    GroupedCtxPtr context;
    //! This is a global bypass, it's less specific than action_bypasses, but then has a wider range of action.
    PropertySetPtr bypass;
    //! This is a local bypass, it allows to override properties of actions by their ID. 
    std::map<int32_t, PropertySetPtr> action_bypasses;
    
    //! Store error report for replies
    ErrorReportPtr report;
    
    //! stores controller data.
    ControllerSpawnPtr spawn;
    
    //! stores workflow for temporary execution.
    std::string workflow_json;
public:
    Request();
    Request(const Target & target);
    Request(const Target & target, const Target & reply);
    virtual ~Request();
    
    static RequestPtr createReply(RequestPtr);
    
    boost::uuids::uuid getRequestId() const;
    boost::uuids::uuid getId() const;
    
    std::string shortRequestId() const;
    std::string logRequest() const;
    
    void setId(const boost::uuids::uuid & id);
    void setRequestId(const boost::uuids::uuid & id);
    
    const std::string & getWorkflowJson() const;
    void setWorkflowJson(const std::string & json);
    
    void setErrorReport(ErrorReportPtr);
    ErrorReportPtr getErrorReport() const;
    
    PropertySetPtr getBypass() const;
    void setBypass(PropertySetPtr);
    
    PropertySetPtr getActionBypasses(int action_id);
    std::map<int32_t, PropertySetPtr> & getActionBypasses();
    
    GroupedCtxPtr getContext() const;
    ContextPtr getContext(const std::string &) const;
    template<class T>
    boost::shared_ptr<T> getCastedContext(const std::string & key) const {
        return boost::dynamic_pointer_cast<T>(getContext(key));
    }
    void setContext(const std::string &, ContextPtr);
    void setContext(const std::string &, Context *);
    
    ControllerSpawnPtr getControllerSpawn() const ;
    void setControllerSpawn(ControllerSpawnPtr);
    
    void setTarget(const Target & );
    void setReply(const Target & );
    
    const Target & getTarget() const;
    const Target & getReply() const;
    
    Target & getTarget();
    Target & getReply();
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
};

OSTREAM_HELPER_DECL(Request);

#pragma GCC visibility pop

#endif // __REQUEST_H_