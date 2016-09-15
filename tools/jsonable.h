#ifndef __JSONABLE_H_
#define __JSONABLE_H_
#include <boost/property_tree/ptree.hpp>
#include <tools/defines.h>

#pragma GCC visibility push(default)
/**
 This class allow other to be serialized to JSON format.
 Seek boost property tree doc for more infos.
 
 Simply override save() and load().
 */
class Jsonable  {
public:
    Jsonable();
    virtual ~Jsonable();
    
    //! store content of this class to a json string.
    void str_save(std::string &) const;
    //! load the content of this class from a json string
    bool str_load(const std::string &);
    
    //! this does the actual class to ptree conversion
    virtual void save(boost::property_tree::ptree & root) const =0;
    //! this convert a string to a ptree
    virtual void load(const boost::property_tree::ptree & root) =0;
    
};

#define GET_OPT(tree, member, type, key) \
{ \
    auto __CAT(o,member) = tree.get_optional<type>(key); \
    if(__CAT(o,member)) member = * __CAT(o,member); \
}

#define PUT_CHILD(tree, member, key) \
{ \
    boost::property_tree::ptree cmember;\
    (member).save(cmember); \
    tree.add_child(key, cmember);\
}

#define GET_CHILD(tree, member, key) \
{ \
    auto popt = tree.get_child_optional(key); \
    if(popt) \
        (member).load(*popt); \
}

#pragma GCC visibility pop

#endif // __JSONABLE_H_