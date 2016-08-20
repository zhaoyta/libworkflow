#ifndef __REQUEST_H_
#define __REQUEST_H_

#include <tools/jsonable.h>
#include <tools/defines.h>
#include <core/target.h>
#include <boost/uuids/uuid.hpp>
#include <core/property_set.h>
#include <map>

SHARED_PTR(Request);
SHARED_PTR(Context);

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
    //! If not null, this tell that the reply should be funneled to a client.
    boost::uuids::uuid client_id;
    //! this is initial (or callbacked) informations to be pushed to the workflow.
    ContextPtr context;
    //! This is a global bypass, it's less specific than action_bypasses, but then has a wider range of action.
    PropertySet bypass;
    //! This is a local bypass, it allows to override properties of actions by their ID. 
    std::map<int32_t, PropertySet> action_bypasses;
pbulic:
    Request();
    Request(const Target & target);
    Request(const Target & target, const Target & reply);
    virtual ~Request();
    
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
};


#endif // __REQUEST_H_