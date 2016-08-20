#ifndef __PROPERTY_SET_H_
#define __PROPERTY_SET_H_

#include <tools/jsonable.h>

class PropertySet: public Jsonable {
public:
    PropertySet();
    virtual ~PropertySet();
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
    
};

#endif // __PROPERTY_SET_H_