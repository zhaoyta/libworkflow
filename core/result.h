#ifndef __RESULT_H_
#define __RESULT_H_

#include <tools/error_report.h>

#pragma GCC visibility push(default)
/**
    Stores action result.
 */
struct Result {
    //! action that finishes.
    int32_t action_id;
    
    //! result type
    enum Type: uint32_t {
        Done,
        Wait,
        Error,
        Finish,
        Async
    } type;
    
    //! well if result is an error, then it need to be reported here.
    ErrorReportPtr error;
};

#pragma GCC visibility pop


#endif // __RESULT_H_