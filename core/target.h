#ifndef __TARGET_H_
#define __TARGET_H_
#include <tools/defines.h>
#include <tools/jsonable.h>

#pragma GCC visibility push(default)

/**
    Define what to do :)
 */
struct Target: public Jsonable {
    //! what is the targeted controller. If left empty, the default controller will be used (same with "default")
    std::string controller = "default";
    //! specify which workflow shall be started.
    std::string workflow;
    //! This is the unique id by which this instance will be called.
    boost::uuids::uuid id;
    
    enum TargetAction : uint32_t {
        DefaultAction, //!< This, according state machine state, will either, start the workflow from the begining, or continue it.
        Status, //!< This is a blind state, which allow access to Session ( if an id has been provided ) OR access to internal Workflow data.
        Interrupt, //!< This will call the interrupt state
        Error //!< This prompt the Error state, unless expected action can handle Error. 
    } target = DefaultAction;
    

    bool operator<(const Target & t);
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
    
};


OSTREAM_HELPER_DECL(Target);


#pragma GCC visibility pop

#endif // __TARGET_H_