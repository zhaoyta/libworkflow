#ifndef __STEPS_H_
#define __STEPS_H_

enum class Step : int32_t {
    Die = - 1666, //!< this step, once reached, declare the end of the session usage.
    Cleanup = -1020, //!< this is the last state in which the state machine can get.
    Error = -1015, //!< Error state is triggered when either, an action report an error, when an action fails to get executed, or doesn't meet outputs requirements, or when an async request comes back with error. Default replies with error to the caller.
    Finish = -1010, //!< Finish is the natural, OK ending of a state machine.
    Interrupt = -1005, //!< This state interrupt the flow of execution, aborting all current execution and ignoring async one as well.
    Status = -1000, //!< This special state allow one to access non session related stuff.
};

#endif // __STEPS_H_