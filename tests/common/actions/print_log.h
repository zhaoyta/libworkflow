#ifndef __PRINT_LOG_H_
#define __PRINT_LOG_H_

#include <core/action.h>

/**
 Simply do nothing, return done(). Print on log stored in property log.
 */
class PrintLog: public Action {
public:
    PrintLog();
    Result perform(SessionPtr session) const override;
};

#endif // __PRINT_LOG_H_