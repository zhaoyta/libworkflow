#ifndef __PROPERTY_H_
#define __PROPERTY_H_

#include <tools/jsonable.h>
#include <core/context.h>
#include <string>
#include <tools/logged.h>
#include <tools/context_factory.h>

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
    
    GLOB_LOGGER("property");
    BOOST_LOG_SEV(logger, Error) << " Invalid property type serialization requested.";
}

template<>
void Property<bool>::save(boost::property_tree::ptree & root) const {
    root.put("key", key);
    root.put("value", value);
    root.put("description", description);
    root.put("exposed", exposed);
}

template<>
void Property<std::string>::save(boost::property_tree::ptree & root) const {
    root.put("key", key);
    root.put("value", value);
    root.put("description", description);
    root.put("exposed", exposed);
}

template<>
void Property<uint32_t>::save(boost::property_tree::ptree & root) const {
    root.put("key", key);
    root.put("value", value);
    root.put("description", description);
    root.put("exposed", exposed);
}

template<>
void Property<double>::save(boost::property_tree::ptree & root) const {
    root.put("key", key);
    root.put("value", value);
    root.put("description", description);
    root.put("exposed", exposed);
}

template<>
void Property<ContextPtr>::save(boost::property_tree::ptree & root) const {
    root.put("key", key);
    root.put("description", description);
    root.put("exposed", exposed);
    if(value) {
        PUT_CHILD(root, *value, "value");
    }
}

template<typename T>
void Property<T>::load(const boost::property_tree::ptree & root) {
    // Log: unable to store this ... well, shouldn't happend anyway :)
    
    GLOB_LOGGER("property");
    BOOST_LOG_SEV(logger, Error) << " Invalid property type deserialization requested.";
}

template<>
void Property<bool>::load(const boost::property_tree::ptree & root) {
    GET_OPT(root, key, std::string, "key");
    GET_OPT(root, value, bool, "value");
    GET_OPT(root, description, std::string, "description");
    GET_OPT(root, exposed, bool, "exposed");
}

template<>
void Property<std::string>::load(const boost::property_tree::ptree & root) {
    GET_OPT(root, key, std::string, "key");
    GET_OPT(root, value, std::string, "value");
    GET_OPT(root, description, std::string, "description");
    GET_OPT(root, exposed, bool, "exposed");
}

template<>
void Property<uint32_t>::load(const boost::property_tree::ptree & root) {
    GET_OPT(root, key, std::string, "key");
    GET_OPT(root, value, uint32_t, "value");
    GET_OPT(root, description, std::string, "description");
    GET_OPT(root, exposed, bool, "exposed");
}

template<>
void Property<double>::load(const boost::property_tree::ptree & root) {
    GET_OPT(root, key, std::string, "key");
    GET_OPT(root, value, double, "value");
    GET_OPT(root, description, std::string, "description");
    GET_OPT(root, exposed, bool, "exposed");
}

template<>
void Property<ContextPtr>::load(const boost::property_tree::ptree & root) {
    GET_OPT(root, key, std::string, "key");
    GET_OPT(root, description, std::string, "description");
    GET_OPT(root, exposed, bool, "exposed");
    auto cvalue = root.get_child_optional("value");
    if(cvalue) {
        std::string type;
        GET_OPT((*cvalue), type, std::string, "type");
        auto ctx = ContextFactory::create(type);
        if(ctx) {
            ctx->load(*cvalue);
        } else {
            GLOB_LOGGER("property");
            BOOST_LOG_SEV(logger, Error) << " Unable to find appropriate context for type: " << type;
        }
    }
}

#endif /* property_hpp */
