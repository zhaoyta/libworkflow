#ifndef __DO_NEXT_H_
#define __DO_NEXT_H_

#include <core/action.h>

/**
 Simply do nothing, return done(). Print on log Do next performed.
 */
class DoNext: public Action {
public:
    DoNext();
    Result perform(SessionPtr session) const override;
};

#endif // __DO_NEXT_H_