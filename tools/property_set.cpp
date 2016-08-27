#include <tools/property_set.h>
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
}

std::string PropertySet::getDescription(const std::string &) const {
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
}

// #########################
// ##### BOOL PROPERTIES ###
// #########################

bool PropertySet::getBoolValue(const std::string &key) const {
    bool defaultv = false;
    return getBoolValue(key,defaulv);
}

bool PropertySet::getBoolValue(const std::string &key, const bool & default_value) const {
    bool is_present = false;
    return getBoolValue(key,default_value,is_present);
}

bool PropertySet::getBoolValue(const std::string &key, const bool & default_value, bool & is_present) const {
    is_present = false;
    if(bool_values.count(key)> 0) {
        is_present = true;
        return bool_values.at(key).value;
    }
    return default_value;
}

void PropertySet::defineBoolValue(const std::string & key, const bool & value, const std::string & description, bool exposed) {
    Property<bool> prop;
    prop.key = key;
    prop.value = value;
    prop.type = Bool;
    prop.description = description;
    prop.exposed = exposed;
    
    bool_values[key] = property;
}

bool PropertySet::setBoolValue(const std::string & key, const bool & value) {
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

double PropertySet::getDoubleValue(const std::string &key) const {
    double defaultv = 0;
    return getDoubleValue(key,defaulv);
}

double PropertySet::getDoubleValue(const std::string &key, const double & default_value) const {
    bool is_present = false;
    return getDoubleValue(key,default_value,is_present);
}

double PropertySet::getDoubleValue(const std::string &key, const double & default_value, bool & is_present) const {
    is_present = false;
    if(double_values.count(key)> 0) {
        is_present = true;
        return double_values.at(key).value;
    }
    return default_value;
}

void PropertySet::defineDoubleValue(const std::string & key, const double & value, const std::string & description, bool exposed) {
    Property<double> prop;
    prop.key = key;
    prop.value = value;
    prop.type = Double;
    prop.description = description;
    prop.exposed = exposed;
    
    double_values[key] = property;
}

double PropertySet::setDoubleValue(const std::string & key, const double & value) {
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

uint32_t PropertySet::getUintValue(const std::string &key) const {
    uint32_t defaultv = 0;
    return getUintValue(key,defaulv);
}

uint32_t PropertySet::getUintValue(const std::string &key, const uint32_t & default_value) const {
    bool is_present = false;
    return getUintValue(key,default_value,is_present);
}

uint32_t PropertySet::getUintValue(const std::string &key, const uint32_t & default_value, bool & is_present) const {
    is_present = false;
    if(custom_values.count(key)> 0) {
        is_present = true;
        return uint_values.at(key).value;
    }
    return default_value;
}

void PropertySet::defineUintValue(const std::string & key, const uint32_t & value, const std::string & description, bool exposed) {
    Property<uint32_t> prop;
    prop.key = key;
    prop.value = value;
    prop.type = Double;
    prop.description = description;
    prop.exposed = exposed;
    
    uint_values[key] = property;
}

uint32_t PropertySet::setUintValue(const std::string & key, const uint32_t & value) {
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

std::string PropertySet::getStringValue(const std::string &key) const {
    std::string defaultv;
    return getStringValue(key,defaulv);
}

std::string PropertySet::getStringValue(const std::string &key, const std::string & default_value) const {
    bool is_present = false;
    return getStringValue(key,default_value,is_present);
}

std::string PropertySet::getStringValue(const std::string &key, const std::string & default_value, bool & is_present) const {
    is_present = false;
    if(std::stringvalues.count(key)> 0) {
        is_present = true;
        return std::stringvalues.at(key).value;
    }
    return default_value;
}

void PropertySet::defineStringValue(const std::string & key, const std::string & value, const std::string & description, bool exposed) {
    Property<std::string> prop;
    prop.key = key;
    prop.value = value;
    prop.type = Double;
    prop.description = description;
    prop.exposed = exposed;
    
    std::stringvalues[key] = property;
}

std::string PropertySet::setStringValue(const std::string & key, const std::string & value) {
    if(not guarded or (guarded and std::stringvalues.count(key) > 0 )) {
        std::stringvalues[key].value = value;
        return true;
    } else {
        // Log: unable to set value of a guarded and undefined property.
        return false;
    }
}


// #############################
// ##### CUSTOM PROPERTIES #####
// #############################

ContextPtr PropertySet::getCustomValue(const std::string &key) const {
    ContextPtr defaultv;
    return getCustomValue(key,defaulv);
}

ContextPtr PropertySet::getCustomValue(const std::string &key, const ContextPtr & default_value) const {
    bool is_present = false;
    return getCustomValue(key,default_value,is_present);
}

ContextPtr PropertySet::getCustomValue(const std::string &key, const ContextPtr & default_value, bool & is_present) const {
    is_present = false;
    if(custom_values.count(key)> 0) {
        is_present = true;
        return custom_values.at(key).value;
    }
    return default_value;
}

void PropertySet::defineCustomValue(const std::string & key, const ContextPtr & value, const std::string & description, bool exposed) {
    Property<ContextPtr> prop;
    prop.key = key;
    prop.value = value;
    prop.type = Double;
    prop.description = description;
    prop.exposed = exposed;
    
    custom_values[key] = property;
}

ContextPtr PropertySet::setCustomValue(const std::string & key, const ContextPtr & value) {
    if(not guarded or (guarded and custom_values.count(key) > 0 )) {
        custom_values[key].value = value;
        return true;
    } else {
        // Log: unable to set value of a guarded and undefined property.
        return false;
    }
}



void PropertySet::save(boost::property_tree::ptree & root) const {
    
}

void PropertySet::load(const boost::property_tree::ptree & root) {
    
}
