#ifndef __REQUEST_H_
#define __REQUEST_H_

#include <tools/jsonable.h>
#include <tools/defines.h>
#include <core/target.h>
#include <boost/uuids/uuid.hpp>
#include <map>

SHARED_PTR(Request);
SHARED_PTR(Context);
SHARED_PTR(PropertySet);

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
    //! If not null, this tell that the reply should be funneled to a client.
    boost::uuids::uuid client_id;
    //! this is initial (or callbacked) informations to be pushed to the workflow.
    ContextPtr context;
    //! This is a global bypass, it's less specific than action_bypasses, but then has a wider range of action.
    PropertySetPtr bypass;
    //! This is a local bypass, it allows to override properties of actions by their ID. 
    std::map<int32_t, PropertySetPtr> action_bypasses;
pbulic:
    Request();
    Request(const Target & target);
    Request(const Target & target, const Target & reply);
    virtual ~Request();
    
    static RequestPtr reply(RequestPtr);
    
    boost::uuids::uuid getClientId() const;
    boost::uuids::uuid getRequestId() const;
    boost::uuids::uuid getId() const;
    
    std::string shortRequestId() const;
    
    void setId(const boost::uuids::uuid & id);
    void setRequestId(const boost::uuids::uuid & id);
    void setClientId(const boost::uuids::uuid & id);
    
    PropertySetPtr getBypass();
    std::map<int32_t, PropertySetPtr> & getActionBypasses();
    
    ContextPtr getContext();
    template<class T>
    boost::shared_ptr<T> getCastedContext() {
        return boost::dynamic_pointer_cast<T>(getContext());
    }
    
    const Target & getTarget() const;
    const Target & getReply() const;
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
};

OSTREAM_HELPER_DECL(Request);

#endif // __REQUEST_H_