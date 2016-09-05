#ifndef __ERROR_H_
#define __ERROR_H_

#include <core/action.h>

/**
 Will result in an error, error message can be provided through property.
 */
class ErrorAction: public Action {
public:
    ErrorAction();
    Result perform(SessionPtr session) const override;
};

#endif // __DO_NEXT_H_