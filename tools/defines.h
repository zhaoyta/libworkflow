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

#endif // __SHARED_PTR_H_