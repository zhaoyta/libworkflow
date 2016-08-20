#ifndef __ACTION_META_H_
#define __ACTION_META_H_
#include <tools/jsonable.h>
#include <boost/uuids/uuid.hpp>

#pragma GCC visibility push(default)

/**
 This stores meta data,
 */
class ActionMeta : public Jsonable {
    std::string description;
    
    std::map<boost::uuids::uuid, Target> sub_requests;
    std::map<boost::uuids::uuid, 
    
public:
    ActionMeta();
    virtual ~Actionmeta();
    
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
};

#pragma GCC visibility pop

#endinf // __ACTION_META_H_