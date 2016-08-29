#ifndef __TIMED_H_
#define __TIMED_H_

#include <tools/defines.h>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/chrono.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#pragma GCC visibility push(default)
SHARED_PTR(Timed);
typedef boost::shared_ptr<boost::asio::deadline_timer> TimerPtr;
typedef boost::shared_ptr<boost::asio::io_service> IOServicePtr;


/**
 Timed is a class that stores time and bind timer ... :)
 Upon creation, it store current time. 
 
 but you may use it to trigger timerelated callback.
 */
class Timed {
    TimerPtr timer;
    boost::chrono::high_resolution_clock::time_point tp_start;
    boost::function<void(const boost::system::error_code&)> timeoutFunction;
    
    double ms_timeout;
public:
    Timed();
    virtual ~Timed();
    
    //! reset start point for elapsed.
    void reset();
    
    //! set for how long the timer should wait.
    void setDuration(double ms);
    
    //! start timer;
    void start();
    //! stop timer;
    void stop();
    //! set which callback's gonna be called.
    void setTimeoutFunction(const boost::function<void(const boost::system::error_code&)> & to);
    //! without an ioservice, timer can't fire.
    void setIOService(IOServicePtr);
    
    
    //! @return ms since last reset.
    double elapsed();
    
};


#pragma GCC visibility pop

#endif // __TIMED_H_