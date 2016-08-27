#ifndef __PROPERTY_H_
#define __PROPERTY_H_

#include <tools/jsonable.h>
#include <string>

/**
    Define a Property of a PropertySet.
 Property is a key, a value, a description, and tell whether this value is exposed to the world or not.
 */
template<typename T>
class Property : public Jsonable{
public:
    enum Type: uint32_t {
        Uint,
        Bool,
        String,
        Double,
        Custom
    } type;
    
    std::string key;
    //! Actual value.
    T value;
    //! What this value imply
    std::string description;
    //! Should this value be exposed outside it's action current worklow.
    bool exposed;
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
};

template<typename T>
void Property::save(boost::property_tree::ptree & root) const {
    // Log: unable to store this ... well, shouldn't happend anyway :)
}

template<>
void Property::save<bool>(boost::property_tree::ptree & root) const {
    
}

template<>
void Property::save<std::string>(boost::property_tree::ptree & root) const {
    
}

template<>
void Property::save<uint32_t>(boost::property_tree::ptree & root) const {
    
}

template<>
void Property::save<double>(boost::property_tree::ptree & root) const {
    
}

template<>
void Property::save<ContextPtr>(boost::property_tree::ptree & root) const {
    
}

template<typename T>
void Property::load(const boost::property_tree::ptree & root) {
    // Log: unable to store this ... well, shouldn't happend anyway :)
}

template<>
void Property::load<bool>(const boost::property_tree::ptree & root) {
    
}

template<>
void Property::load<std::string>(const boost::property_tree::ptree & root) {
    
}

template<>
void Property::load<uint32_t>(const boost::property_tree::ptree & root) {
    
}

template<>
void Property::load<double>(const boost::property_tree::ptree & root) {
    
}

template<>
void Property::load<ContextPtr>(const boost::property_tree::ptree & root) {
    
}

#endif /* property_hpp */
