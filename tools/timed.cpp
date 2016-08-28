#include <tools/timed.h>

using namespace boost::chrono;

Timed::Timed() {
    tp_start = high_resolution_clock::now();
    ms_timeout = 0;
}

Timed::~Timed() {
    
}

void Timed::reset() {
    
}

void Timed::setDuration(double ms) {
    ms_timeout = ms;
}

void Timed::start() {
    if(timer) {
        timer->expires_from_now(boost::posix_time::milliseconds(ms_timeout));
        timer->async_wait(timeoutFunction);
    }
}

void Timed::stop() {
    if(timer) {
        timer->cancel();
    }
}

void Timed::setIOService(IOServicePtr service) {
    timer.reset(new boost::asio::deadline_timer(*service));
}

void Timed::setTimeoutFunction(const boost::function<void(const boost::system::error_code&)> & to) {
    timeoutFunction = to;
}

double Timed::elapsed() {
    return (high_resolution_clock::now() - tp_start).count() * ((double)high_resolution_clock::period::num/high_resolution_clock::period::den);
}