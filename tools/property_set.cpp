#include <tools/property_set.h>
#include <tools/property.h>
#include <core/context.h>

PropertySet::PropertySet() : Jsonable(), guarded(false) {
    setGuarded(false);
}

PropertySet::~PropertySet() {}

void PropertySet::setGuarded(bool b) {
    guarded = b;
}

bool PropertySet::hasProperty(const std::string & key) const {
    return
    bool_values.count(key) > 0 or
    double_values.count(key) > 0 or
    uint_values.count(key) > 0 or
    string_values.count(key) > 0 or
    custom_values.count(key) > 0;
}

bool PropertySet::isExposed(const std::string & key) const {
    if(bool_values.count(key)> 0 ) {
        return bool_values.at(key).exposed;
    }
    if(double_values.count(key)> 0 ) {
        return double_values.at(key).exposed;
    }
    if(uint_values.count(key)> 0 ) {
        return uint_values.at(key).exposed;
    }
    if(string_values.count(key)> 0 ) {
        return string_values.at(key).exposed;
    }
    if(custom_values.count(key)> 0 ) {
        return custom_values.at(key).exposed;
    }
    return false;
}

std::string PropertySet::getDescription(const std::string & key) const {
    if(bool_values.count(key)> 0 ) {
        return bool_values.at(key).description;
    }
    if(double_values.count(key)> 0 ) {
        return double_values.at(key).description;
    }
    if(uint_values.count(key)> 0 ) {
        return uint_values.at(key).description;
    }
    if(string_values.count(key)> 0 ) {
        return string_values.at(key).description;
    }
    if(custom_values.count(key)> 0 ) {
        return custom_values.at(key).description;
    }
    return "";
}

// #########################
// ##### BOOL PROPERTIES ###
// #########################

bool PropertySet::getBoolProperty(const std::string &key) const {
    bool defaultv = false;
    return getBoolProperty(key,defaultv);
}

bool PropertySet::getBoolProperty(const std::string &key, const bool & default_value) const {
    bool is_present = false;
    return getBoolProperty(key,default_value,is_present);
}

bool PropertySet::getBoolProperty(const std::string &key, const bool & default_value, bool & is_present) const {
    is_present = false;
    if(bool_values.count(key)> 0) {
        is_present = true;
        return bool_values.at(key).value;
    }
    return default_value;
}

void PropertySet::defineBoolProperty(const std::string & key, const bool & value, const std::string & description, bool exposed) {
    Property<bool> prop;
    prop.key = key;
    prop.value = value;
    prop.type = EPropertyType::Bool;
    prop.description = description;
    prop.exposed = exposed;
    
    bool_values[key] = prop;
}

bool PropertySet::setBoolProperty(const std::string & key, const bool & value) {
    if(not guarded or (guarded and bool_values.count(key) > 0 )) {
        bool_values[key].value = value;
        return true;
    } else {
        // Log: unable to set value of a guarded and undefined property.
        return false;
    }
}


// ###########################
// ##### DOUBLE PROPERTIES ###
// ###########################

double PropertySet::getDoubleProperty(const std::string &key) const {
    double defaultv = 0;
    return getDoubleProperty(key,defaultv);
}

double PropertySet::getDoubleProperty(const std::string &key, const double & default_value) const {
    bool is_present = false;
    return getDoubleProperty(key,default_value,is_present);
}

double PropertySet::getDoubleProperty(const std::string &key, const double & default_value, bool & is_present) const {
    is_present = false;
    if(double_values.count(key)> 0) {
        is_present = true;
        return double_values.at(key).value;
    }
    return default_value;
}

void PropertySet::defineDoubleProperty(const std::string & key, const double & value, const std::string & description, bool exposed) {
    Property<double> prop;
    prop.key = key;
    prop.value = value;
    prop.type = EPropertyType::Double;
    prop.description = description;
    prop.exposed = exposed;
    
    double_values[key] = prop;
}

bool PropertySet::setDoubleProperty(const std::string & key, const double & value) {
    if(not guarded or (guarded and double_values.count(key) > 0 )) {
        double_values[key].value = value;
        return true;
    } else {
        // Log: unable to set value of a guarded and undefined property.
        return false;
    }
}


// ###########################
// ##### UINT PROPERTIES #####
// ###########################

uint32_t PropertySet::getUintProperty(const std::string &key) const {
    uint32_t defaultv = 0;
    return getUintProperty(key,defaultv);
}

uint32_t PropertySet::getUintProperty(const std::string &key, const uint32_t & default_value) const {
    bool is_present = false;
    return getUintProperty(key,default_value,is_present);
}

uint32_t PropertySet::getUintProperty(const std::string &key, const uint32_t & default_value, bool & is_present) const {
    is_present = false;
    if(custom_values.count(key)> 0) {
        is_present = true;
        return uint_values.at(key).value;
    }
    return default_value;
}

void PropertySet::defineUintProperty(const std::string & key, const uint32_t & value, const std::string & description, bool exposed) {
    Property<uint32_t> prop;
    prop.key = key;
    prop.value = value;
    prop.type = EPropertyType::Uint;
    prop.description = description;
    prop.exposed = exposed;
    
    uint_values[key] = prop;
}

bool PropertySet::setUintProperty(const std::string & key, const uint32_t & value) {
    if(not guarded or (guarded and uint_values.count(key) > 0 )) {
        uint_values[key].value = value;
        return true;
    } else {
        // Log: unable to set value of a guarded and undefined property.
        return false;
    }
}



// #############################
// ##### STRING PROPERTIES #####
// #############################

std::string PropertySet::getStringProperty(const std::string &key) const {
    std::string defaultv;
    return getStringProperty(key,defaultv);
}

std::string PropertySet::getStringProperty(const std::string &key, const std::string & default_value) const {
    bool is_present = false;
    return getStringProperty(key,default_value,is_present);
}

std::string PropertySet::getStringProperty(const std::string &key, const std::string & default_value, bool & is_present) const {
    is_present = false;
    if(string_values.count(key)> 0) {
        is_present = true;
        return string_values.at(key).value;
    }
    return default_value;
}

void PropertySet::defineStringProperty(const std::string & key, const std::string & value, const std::string & description, bool exposed) {
    Property<std::string> prop;
    prop.key = key;
    prop.value = value;
    prop.type = EPropertyType::String;
    prop.description = description;
    prop.exposed = exposed;
    
    string_values[key] = prop;
}

bool PropertySet::setStringProperty(const std::string & key, const std::string & value) {
    if(not guarded or (guarded and string_values.count(key) > 0 )) {
        string_values[key].value = value;
        return true;
    } else {
        // Log: unable to set value of a guarded and undefined property.
        return false;
    }
}


// #############################
// ##### CUSTOM PROPERTIES #####
// #############################

ContextPtr PropertySet::getCustomProperty(const std::string &key) const {
    ContextPtr defaultv;
    return getCustomProperty(key,defaultv);
}

ContextPtr PropertySet::getCustomProperty(const std::string &key, const ContextPtr & default_value) const {
    bool is_present = false;
    return getCustomProperty(key,default_value,is_present);
}

ContextPtr PropertySet::getCustomProperty(const std::string &key, const ContextPtr & default_value, bool & is_present) const {
    is_present = false;
    if(custom_values.count(key)> 0) {
        is_present = true;
        return custom_values.at(key).value;
    }
    return default_value;
}

void PropertySet::defineCustomProperty(const std::string & key, const ContextPtr & value, const std::string & description, bool exposed) {
    Property<ContextPtr> prop;
    prop.key = key;
    prop.value = value;
    prop.type = EPropertyType::Custom;
    prop.description = description;
    prop.exposed = exposed;
    
    custom_values[key] = prop;
}

bool PropertySet::setCustomProperty(const std::string & key, const ContextPtr & value) {
    if(not guarded or (guarded and custom_values.count(key) > 0 )) {
        custom_values[key].value = value;
        return true;
    } else {
        // Log: unable to set value of a guarded and undefined property.
        return false;
    }
}


#define SAVE_PROPERTIES(store) \
{ \
    boost::property_tree::ptree sub; \
    for(const auto & kv: store) { \
        boost::property_tree::ptree item; \
        kv.second.save(item); \
        sub.add_child(kv.first, item); \
    } \
    root.add_child(#store, sub); \
}

void PropertySet::save(boost::property_tree::ptree & root) const {
    root.put("guarded", guarded);
    SAVE_PROPERTIES(uint_values);
    SAVE_PROPERTIES(double_values);
    SAVE_PROPERTIES(string_values);
    SAVE_PROPERTIES(bool_values);
    SAVE_PROPERTIES(custom_values);
}

#define LOAD_PROPERTIES(store, type) \
{ \
  auto cmap = root.get_child_optional(#store); \
  if(cmap) { \
    for(const auto & kv: *cmap) { \
        Property<type> p; \
        p.load(kv.second); \
        store[kv.first.data()] = p; \
    } \
  } \
}

void PropertySet::load(const boost::property_tree::ptree & root) {
    GET_OPT(root, guarded, bool, "guarded");
    LOAD_PROPERTIES(uint_values, uint32_t);
    LOAD_PROPERTIES(double_values, double);
    LOAD_PROPERTIES(string_values, std::string);
    LOAD_PROPERTIES(bool_values, bool);
    LOAD_PROPERTIES(custom_values, ContextPtr);
}
