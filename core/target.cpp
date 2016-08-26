#include <core/target.h>

bool Target::operator<(const Target & t) {
    return id < t.id;
}

void Target::save(boost::property_tree::ptree & root) const {
    
}

void Target::load(const boost::property_tree::ptree & root) {
    
}