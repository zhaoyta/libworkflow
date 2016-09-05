#ifndef __SOME_CONTEXT_H_
#define __SOME_CONTEXT_H_

#include <core/context.h>

SHARED_PTR(SomeContext);

/**
 This simplest of context only store a double member.
 */
class SomeContext: public Context {
public:
    SomeContext();
    virtual ~SomeContext();
    
    double some_value;
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
};

#endif // __SOME_CONTEXT_H_