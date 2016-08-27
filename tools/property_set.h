#ifndef __PROPERTY_SET_H_
#define __PROPERTY_SET_H_
#include <map>
#include <tools/jsonable.h>
#include <tools/defines.h>

SHARED_PTR(PropertySet);
SHARED_PTR(Context);

template<typename T> class Property;

/** Helper to declare all appropriate property functions for each type of data.
 
    Declares:
 type getTypeValue(const std::string &key) const;
 type getTypeValue(const std::string &key, const type & default_value) const;
 type getTypeValue(const std::string &key, const type & default_value, bool & is_present) const;
 
 void defineTypeValue(const std::string & key, const type & value, const std::string & description, bool exposed = false);
 
 //! @return false if the propertyset is guarded and value hasn't been defined.
 bool setTypeValue(const std::string & key, const type & value);
 
 */
#define DECLARE_PROPERTY_FUNCTIONS(Name,type) \
type __CAT(__CAT(get,Name),Value)(const std::string & key) const; \
type __CAT(__CAT(get,Name),Value)(const std::string & key, const type & default_value) const; \
type __CAT(__CAT(get,Name),Value)(const std::string & key, const type & default_value, bool & is_present) const; \
\
void __CAT(__CAT(define,Name),Property)(const std::string & key, const type & value, const std::string & description, bool exposed = false); \
\
bool __CAT(__CAT(set,Name),Property)(const std::string & key, const type & value);

/**
 Stores various kind of data, accessible by string keys. 
 */
class PropertySet : public Jsonable {
    //! This ensure that set values check that these values have been defined before hand. That's a safeguard for Action properties.
    bool guarded = false;
    
    
    std::map<std::string, Property<uint32_t> > uint_values;
    std::map<std::string, Property<double> > double_values;
    std::map<std::string, Property<bool> > bool_values;
    std::map<std::string, Property<std::string> > string_values;
    std::map<std::string, Property<ContextPtr> > custom_values;
    
public:
    PropertySet();
    virtual ~PropertySet();
    
    bool hasProperty(const std::string &) const;
    bool isExposed(const std::string &) const;
    std::string getDescription(const std::string &) const;
    
    DECLARE_PROPERTY_FUNCTIONS(Bool, bool);
    DECLARE_PROPERTY_FUNCTIONS(Double, double);
    DECLARE_PROPERTY_FUNCTIONS(String, std::string);
    DECLARE_PROPERTY_FUNCTIONS(Uint, uint32_t);
    DECLARE_PROPERTY_FUNCTIONS(Custom, ContextPtr);
    
    template<class T>
    boost::shared_ptr<T> getCustomCastedValue(const std::string & key, boost::shared_ptr<T> default_value = boost::shared_ptr<T>()) {
        return boost::dynamic_pointer_cast<T>(getCustomValue(key, default_value));
    }
    
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
    
};


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
#endif // __PROPERTY_SET_H_