#ifndef __ACTION_FACTORY_H_
#define __ACTION_FACTORY_H_

#include <map>
#include <string>
#include <tools/defines.h>
#include <core/action.h>

class _ActionBuilder;

template<class T>
class ActionBuilder;

class ActionFactory {
    static std::map<std::string, _ActionBuilder *> builders;
public:
    
    static void registerAction(_ActionBuilder *);
    static ActionPtr create(const std::string & name);
};

class _ActionBuilder {
public:
    
    virtual ActionPtr create();
    std::string getName();
};

template<class T>
class ActionBuilder: public _ActionBuilder {
public:
    ActionPtr create() override {
        ActionPtr act(new T());
        return act;
    }
};

#endif // __ACTION_FACTORY_H_