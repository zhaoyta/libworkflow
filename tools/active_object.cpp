#include <tools/active_object.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>


ActiveObject::ActiveObject(const std::string &name, uint32_t pool, bool delay_start):
    boost::enable_shared_from_this<ActiveObject>(),
    Logged("act.obj"),
    name(name),
    mutex(new boost::recursive_mutex()),
    thread_pool(pool)
{
  if(not delay_start)
      start();
}

ActiveObject::~ActiveObject() {
    
}

const std::string & ActiveObject::getName() const {
    return name;
}

IOServicePtr ActiveObject::getIOService() {
    return service;
}

void ActiveObject::start() {
    boost::interprocess::scoped_lock<boost::recursive_mutex> sl(*mutex);
    if( threads.size() == 0 ) {
        BOOST_LOG_SEV(logger, Info) << getName() << " Starting active object ...";
        boost::shared_ptr<boost::thread> thread(new boost::thread(&ActiveObject::run, this));
        threads.push_back(thread);
    }
}

void ActiveObject::stop() {
    BOOST_LOG_SEV(logger, Info) << getName() << " Stopping active object ...";
    boost::interprocess::scoped_lock<boost::recursive_mutex> sl(*mutex);
    worker.reset();
    service->stop();
}

void ActiveObject::terminate() {
    stop();
    for(auto thread: threads) {
        BOOST_LOG_SEV(logger, Trace) << getName() << " Joining thread ...";
        thread->join();
        BOOST_LOG_SEV(logger, Trace) << getName() << " Thread joined ...";
        
    }
    BOOST_LOG_SEV(logger, Debug) << getName() << " All Thread joined ...";
}

void ActiveObject::started() {
    
}

void ActiveObject::stopped() {
    
}

void ActiveObject::setStoppedFunction(boost::function<void(ActiveObjectPtr)> fn) {
    stop_function = fn;
}

void ActiveObject::setStartedFunction(boost::function<void(ActiveObjectPtr)> fn) {
    BOOST_LOG_SEV(logger, Trace) << getName()  <<" Setting start function";
    start_function= fn;
}

void ActiveObject::run() {
    BOOST_LOG_SEV(logger, Info) << getName()  <<" Starting active object ... Run called";
    service.reset( new boost::asio::io_service());
    started();
    if(start_function) {
        BOOST_LOG_SEV(logger, Debug) << getName() <<" Starting active object ... calling start_function";
        start_function(shared_from_this());
    }
    worker.reset(new boost::asio::io_service::work(*service));
    service->post(boost::bind(&ActiveObject::startPool, this));
    service->run();
    BOOST_LOG_SEV(logger, Debug) << getName() <<" Service closing";

    if(stop_function)
        stop_function(shared_from_this());
    stopped();
    service->reset();
}

void ActiveObject::startPool() {
    while(threads.size() < thread_pool) {
        boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&boost::asio::io_service::run, service.get())));
        threads.push_back(thread);
    }
}

size_t ActiveObject::getPoolSize() const {
    return thread_pool;
}

OSTREAM_HELPER_IMPL(ActiveObject, obj) {
    out << "[ActiveObject] name: " << obj.getName();
    return out;
}
