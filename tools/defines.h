#ifndef __SHARED_PTR_H_
#define __SHARED_PTR_H_

#include <boost/shared_ptr.hpp>
#include <boost/ enable_shared_from_this.hpp>
#include <ostream>

//! Concat helper macro.
#define __CAT(A, B) A##B

//! Helper to declare a Boost Shared Pointer, named as KlassPtr
#define SHARED_PTR(Klass) \
class Klass; \
typedef boost::shared_ptr<Klass> __CAT(Klass, Ptr)

//! Helper to declare an ostream operator for provided class.
//! This will handle pointer, shared_ptr and const Klass &.
#define OSTREAM_HELPER_DECL(Klass) \
std::ostream& operator<<(std::ostream&, const Klass &); \
std::ostream& operator<<(std::ostream&, Klass *); \
SHARED_PTR(Klass); \
std::ostream& operator<<(std::ostream&, __CAT(Klass,Ptr)); 

//! Implementation simply need the basic const Klass & version to be implemented.
#define OSTREAM_HELPER_IMPL(Klass, obj) \
std::ostream& operator<<(std::ostream& out, __CAT(Klass,Ptr) obj) { \
    out << *obj; \
    return out; \
} \
\
std::ostream& operator<<(std::ostream& out, Klass * obj) { \
out << *obj; \
return out; \
} \
\
std::ostream& operator<<(std::ostream& out, const Klass & obj)


//! Declare an enum, named EEnum
#define BEGIN_ENUM_DECL(Enum) enum class __CAT(E,Enum) : uint32_t
//! Declare convertionclass, accessible through CEnum
#define END_ENUM_DECL(Enum, DefaultEnum, DefaultString) \
class __CAT(C,Enum) { \
    static std::map<std::string, uint32_t>  __CAT(Enum,Alpha);\
public:\
    static std::string valueToString(uint32_t, const std::string & default = DefaultString); \
    static std::string valueToString(__CAT(E,Enum), const std::string & default = DefaultString); \
    \
    static __(E,Enum) valueFromString(const std::string &, __CAT(E,Enum) default = __CAT(E,Enum)::DefaultEnum); \
    \
    static std::map<std::string, uint32_t> getConvertion(); \
};

//! This begins implementation of all this, First step is to fill the conversion map, string to uint32_t
#define BEGIN_ENUM_IMPL(Enum) std::map<std::string, uint32_t>  __CAT(Enum,Alpha) =


//! @todo add log in every conversion failure.
//! this implement convertion
#define END_ENUM_IMPL(Enum) \
std::string __CAT(C,Enum)::valueToString(uint32_t, const std::string & default) { \
    for(const auto & kv: __CAT(Enum,Alpha)) {\
        if(kv.second == (uint32_t)key)\
            return kv.first;\
    }\
    return default;\
} \
\
std::string __CAT(C,Enum)::valueToString(__CAT(E,Enum) key) { \
    for(const auto & kv: __CAT(Enum,Alpha)) {\
        if(kv.second == (uint32_t)key)\
            return kv.first;\
    }\
    return "unknown";\
}\
\
__(E,Enum) __CAT(C,Enum)::valueFromString(const std::string & key, __CAT(E,Enum) default) { \
    if(getConvertion().count(key))\
        return (__CAT(E,Enum))getConvertion().at(key);\
    return default; \
} \
\
std::map<std::string, uint32_t> __CAT(C,Enum)::getConvertion() { \
    return __CAT(Enum, Alpha); \
} \


#endif // __SHARED_PTR_H_