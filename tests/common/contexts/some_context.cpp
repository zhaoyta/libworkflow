#include <tests/common/contexts/some_context.h>



SomeContext::SomeContext() : Context("SomeContext"), some_value(0.0) {
    
}

SomeContext::~SomeContext() {}

void SomeContext::save(boost::property_tree::ptree & root) const {
    root.put("some_value", some_value);
}

void SomeContext::load(const boost::property_tree::ptree & root) {
    GET_OPT(root, some_value, double, "some_value");
}