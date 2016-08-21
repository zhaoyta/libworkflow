#ifndef __TYPE_CHECKER_H_
#define __TYPE_CHECKER_H_

#include <core/context.h>
#include <tools/shared_ptr.h>

SHARED_PTR(TypeChecker);

/**
    This checks that a provided context is the right one. Default version isn't specialized thus is always happy.
    @sa ContextTypeChecker
 */
class TypeChecker {
public:
    TypeChecker(){}
    virtual ~TypeChecker() {}
    
    //! This allow checker to generate an empty version of the expected context.
    virtual ContextPtr generate() const {
        return ContextPtr(new Context());
    }
    
    //! This does the actual check
    //! @return true if provided @a _ctx matches the one expected.
    virtual bool check(ContextPtr _ctx) const {
        return true;
    }
};

SHARED_PTR(ContextTypeChecker);

/**
    This checks against a Context Type, if provided context matches the one expected, then it succeed.
 */
template <class T>
class ContextTypeChecker {
public:
    ContextTypeChecker() {}
    virtual ~ContextTypeChecker() {}
    
    ContextPtr generate() const override {
        return boost::shared_ptr<T>(new T());
    }
    
    bool check(ContextPtr ctx) const override {
        if(ctx) {
            if(boost::dynamic_pointer_cast<T>(ctx))
                return true;
        }
        return false;
    }
    
};

#define CHECKER(Klass) TypeCheckerPtr(new ContextTypeChecker<Klass>())

#endif // __TYPE_CHECKER_H_