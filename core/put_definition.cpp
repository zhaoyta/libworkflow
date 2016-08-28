#include <core/put_definition.h>

PutDefinition::PutDefinition() :
    mandatory(true),
    ignoreEmpty(false),
allowSkip(false) {}
PutDefinition::~PutDefinition() {}


bool PutDefinition::operator<(const PutDefinition & pd) const {
    return put_name < pd.put_name;
}

void PutDefinition::save(boost::property_tree::ptree & root) const {
    
}

void PutDefinition::load(const boost::property_tree::ptree & root) {
    
}
