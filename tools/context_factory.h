#ifndef __CONTEXT_FACTORY_H_
#define __CONTEXT_FACTORY_H_

#include <tools/defines.h>
#include <core/context.h>
#include <map>

class _ContextBuilder;
template<class T> class ContextBuilder;

/**
 Used for json serialization
 Allow to create an empty shell of context based on it's type.
 */
class ContextFactory {
    ContextFactory();
    static std::map<std::string, _ContextBuilder * > builders;
public:
    
    static void registerContext(_ContextBuilder * cbuild);
    static ContextPtr create(const std::string & type);
};

class _ContextBuilder {
public:
    virtual ContextPtr create();
    std::string getName();
};

template<class T>
class ContextBuilder : public _ContextBuilder {
public:
    ContextPtr create() override {
        boost::shared_ptr<T> r(new T());
        ContextPtr res = boost::dynamic_pointer_cast<Context>(r);
        return res;
    }
};

#endif // __CONTEXT_FACTORY_H_