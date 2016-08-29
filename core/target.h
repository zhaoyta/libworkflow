#ifndef __TARGET_H_
#define __TARGET_H_
#include <tools/defines.h>
#include <tools/jsonable.h>
#include <boost/uuid/uuid.hpp>

#pragma GCC visibility push(default)
SHARED_PTR(Target);

BEGIN_ENUM_DECL(TargetAction) {
    DefaultAction, //!< This, according state machine state, will either, start the workflow from the begining, or continue it.
    Status, //!< This is a blind state, which allow access to Session ( if an id has been provided ) OR access to internal Workflow data.
    Interrupt, //!< This will call the interrupt state
    Error, //!< This prompt the Error state, unless expected action can handle Error.
    Reply, //!< This tell that we're dealing with a reply, and not a request ;)
    NoReply, //! This ensure that this request gets ignored.
};
END_ENUM_DECL(TargetAction, DefaultAction, "DefaultAction")

/**
 Define what to do :)
 */
class Target : public Jsonable {
public:
    Target();
    virtual ~Target();
    //! what is the targeted controller. If left empty, the default controller will be used (same with "default")
    std::string controller;
    //! specify which workflow shall be started.
    std::string workflow;
    //! This is the unique id by which this instance will be called.
    boost::uuids::uuid identifier;
    
    //! This is mostly usefull for reply, it's checked when the reply gets registered.
    //! if this is different than what's stored in session, this request gets ignored.
    //! note, if set to 0, it'll be accepted nonetheless (default level is 1)
    uint32_t execution_level;
    
    std::string shortId() const;
    
    ETargetAction target;
    
    int32_t action;
    
    bool operator<(const Target & t) const;
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
    
};



OSTREAM_HELPER_DECL(Target);

#pragma GCC visibility pop

#endif // __TARGET_H_