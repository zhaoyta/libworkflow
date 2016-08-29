#ifndef __PROPERTY_SET_H_
#define __PROPERTY_SET_H_
#include <map>
#include <tools/jsonable.h>
#include <tools/defines.h>


#pragma GCC visibility push(default)

SHARED_PTR(PropertySet);
SHARED_PTR(Context);

template<typename T> class Property;

/** Helper to declare all appropriate property functions for each type of data.
 
    Declares:
 type getTypeProperty(const std::string &key) const;
 type getTypeProperty(const std::string &key, const type & default_value) const;
 type getTypeProperty(const std::string &key, const type & default_value, bool & is_present) const;
 
 void defineTypeProperty(const std::string & key, const type & value, const std::string & description, bool exposed = false);
 
 //! @return false if the propertyset is guarded and value hasn't been defined.
 bool setTypeProperty(const std::string & key, const type & value);
 
 */
#define DECLARE_PROPERTY_FUNCTIONS(Name,type) \
type __CAT(__CAT(get,Name),Property)(const std::string & key) const; \
type __CAT(__CAT(get,Name),Property)(const std::string & key, const type & default_value) const; \
type __CAT(__CAT(get,Name),Property)(const std::string & key, const type & default_value, bool & is_present) const; \
\
void __CAT(__CAT(define,Name),Property)(const std::string & key, const type & value, const std::string & description, bool exposed = false); \
\
bool __CAT(__CAT(set,Name),Property)(const std::string & key, const type & value);

/**
 Stores various kind of data, accessible by string keys. 
 */
class PropertySet : public Jsonable {
    //! This ensure that set values check that these values have been defined before hand. That's a safeguard for Action properties.
    bool guarded;
    
    
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
    
    void setGuarded(bool b);
    
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


#pragma GCC visibility pop

#endif // __PROPERTY_SET_H_