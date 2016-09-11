#ifndef __PROPERTY_H_
#define __PROPERTY_H_

#include <tools/jsonable.h>
#include <core/context.h>
#include <string>
#include <tools/logged.h>
#include <tools/context_factory.h>

#pragma GCC visibility push(default)


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
class _Property : public Jsonable{
public:
    
    std::string key;
    //! What this value imply
    std::string description;
    //! Should this value be exposed outside it's action current worklow.
    bool exposed;
    
    EPropertyType type;
};

template<class T> class Property : public _Property{
public:
    T value;
    void save(boost::property_tree::ptree & root) const {
        // Log: unable to store this ... well, shouldn't happend anyway :)
        
        GLOB_LOGGER("property");
        BOOST_LOG_SEV(logger, Error) << " Invalid property type serialization requested.";
    }

    void load(const boost::property_tree::ptree & root) {
        // Log: unable to store this ... well, shouldn't happend anyway :)
        
        GLOB_LOGGER("property");
        BOOST_LOG_SEV(logger, Error) << " Invalid property type deserialization requested.";
    }

};

template<> class Property<bool> : public _Property{
public:
    bool value;
    
    void save(boost::property_tree::ptree & root) const  {
        root.put("key", key);
        root.put("value", value);
        root.put("description", description);
        root.put("exposed", exposed);
    }
    void load(const boost::property_tree::ptree & root) {
        GET_OPT(root, key, std::string, "key");
        GET_OPT(root, value, bool, "value");
        GET_OPT(root, description, std::string, "description");
        GET_OPT(root, exposed, bool, "exposed");
    }

};

template<> class Property<std::string> : public _Property{
public:
    std::string value;
    
    void save(boost::property_tree::ptree & root) const {
        root.put("key", key);
        root.put("value", value);
        root.put("description", description);
        root.put("exposed", exposed);
    }
    void load(const boost::property_tree::ptree & root) {
        GET_OPT(root, key, std::string, "key");
        GET_OPT(root, value, std::string, "value");
        GET_OPT(root, description, std::string, "description");
        GET_OPT(root, exposed, bool, "exposed");
    }
};

template<> class Property<double> : public _Property{
public:
    double value;
    
    void save(boost::property_tree::ptree & root) const {
        root.put("key", key);
        root.put("value", value);
        root.put("description", description);
        root.put("exposed", exposed);
    }

    void load(const boost::property_tree::ptree & root) {
        GET_OPT(root, key, std::string, "key");
        GET_OPT(root, value, double, "value");
        GET_OPT(root, description, std::string, "description");
        GET_OPT(root, exposed, bool, "exposed");
    }
};

template<> class Property<uint32_t> : public _Property{
public:
    uint32_t value;
    
    void save(boost::property_tree::ptree & root) const  {
        root.put("key", key);
        root.put("value", value);
        root.put("description", description);
        root.put("exposed", exposed);
    }

    void load(const boost::property_tree::ptree & root) {
        GET_OPT(root, key, std::string, "key");
        GET_OPT(root, value, uint32_t, "value");
        GET_OPT(root, description, std::string, "description");
        GET_OPT(root, exposed, bool, "exposed");
    }
};

template<> class Property<ContextPtr> : public _Property{
public:
    ContextPtr value;
    
    void save(boost::property_tree::ptree & root) const {
        root.put("key", key);
        root.put("description", description);
        root.put("exposed", exposed);
        if(value) {
            PUT_CHILD(root, *value, "value");
        }
    }

    void load(const boost::property_tree::ptree & root) {
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

};

#pragma GCC visibility pop

#endif // __PROPERTY_H_
