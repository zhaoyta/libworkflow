#ifndef __PROPERTY_H_
#define __PROPERTY_H_

#include <tools/jsonable.h>
#include <core/context.h>
#include <string>

BEGIN_ENUM_DECL(PropertyType) {
        Uint,
        Bool,
        String,
        Double,
        Custom
};
END_ENUM_DECL(PropertyType, Uint, "Uint");

/**
    Define a Property of a PropertySet.
 Property is a key, a value, a description, and tell whether this value is exposed to the world or not.
 */
template<typename T> class Property : public Jsonable{
public:
    
    std::string key;
    //! Actual value.
    T value;
    //! What this value imply
    std::string description;
    //! Should this value be exposed outside it's action current worklow.
    bool exposed;
    
    EPropertyType type;
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
};

template<typename T> void Property<T>::save(boost::property_tree::ptree & root) const {
    // Log: unable to store this ... well, shouldn't happend anyway :)
}

template<>
void Property<bool>::save(boost::property_tree::ptree & root) const {
    
}

template<>
void Property<std::string>::save(boost::property_tree::ptree & root) const {
    
}

template<>
void Property<uint32_t>::save(boost::property_tree::ptree & root) const {
    
}

template<>
void Property<double>::save(boost::property_tree::ptree & root) const {
    
}

template<>
void Property<ContextPtr>::save(boost::property_tree::ptree & root) const {
    
}

template<typename T>
void Property<T>::load(const boost::property_tree::ptree & root) {
    // Log: unable to store this ... well, shouldn't happend anyway :)
}

template<>
void Property<bool>::load(const boost::property_tree::ptree & root) {
    
}

template<>
void Property<std::string>::load(const boost::property_tree::ptree & root) {
    
}

template<>
void Property<uint32_t>::load(const boost::property_tree::ptree & root) {
    
}

template<>
void Property<double>::load(const boost::property_tree::ptree & root) {
    
}

template<>
void Property<ContextPtr>::load(const boost::property_tree::ptree & root) {
    
}

#endif /* property_hpp */
