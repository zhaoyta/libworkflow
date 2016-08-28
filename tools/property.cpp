#include "property.h"


BEGIN_ENUM_IMPL(PropertyType) {
    {"Uint",    (uint32_t)EPropertyType::Uint},
    {"Bool",    (uint32_t)EPropertyType::Bool},
    {"String",  (uint32_t)EPropertyType::String},
    {"Double",  (uint32_t)EPropertyType::Double},
    {"Custom",  (uint32_t)EPropertyType::Custom},
};
END_ENUM_IMPL(PropertyType);