#ifndef __SOME_ACTIONS_H_
#define __SOME_ACTIONS_H_
#include <core/action.h>
#include <tests/common/contexts/some_context.h>

/**
 This test action simply produce an output on some_data with  context SomeContext.
 */
class SomeProducer: public Action {
public:
    SomeProducer();
    Result perform(SessionPtr session) const override;
};

/**
 This action expect an input on some_data with SomeContext.
 */
class SomeConsummer: public Action {
public:
    SomeConsummer();
    Result perform(SessionPtr session) const override;
};


/**
 This action allows an input on some_data with SomeContext.
 */
class SomeAllowed: public Action {
public:
    SomeAllowed();
    Result perform(SessionPtr session) const override;
};



#endif // __SOME_ACTIONS_H_