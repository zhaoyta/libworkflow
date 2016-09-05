#include <tests/common/contexts/some_context.h>



SomeContext::SomeContext() : Context("SomeContext"), some_value(0.0) {
    
}

SomeContext::~SomeContext() {}

void SomeContext::save(boost::property_tree::ptree & root) const {
    
}

void SomeContext::load(const boost::property_tree::ptree & root) {
    
}