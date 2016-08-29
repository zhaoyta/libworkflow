#include <tools/active_object.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>


ActiveObject::ActiveObject(const std::string &name, uint32_t pool, bool delay_start):
    boost::enable_shared_from_this<ActiveObject>(),
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
        std::cout << this << " Starting active object ..." << std::endl;
        boost::shared_ptr<boost::thread> thread(new boost::thread(&ActiveObject::run, this));
        threads.push_back(thread);
    }
}

void ActiveObject::stop() {
    boost::interprocess::scoped_lock<boost::recursive_mutex> sl(*mutex);
    worker.reset();
}

void ActiveObject::terminate() {
    boost::interprocess::scoped_lock<boost::recursive_mutex> sl(*mutex);
    stop();
    for(auto thread: threads)
        thread->join();
}

void ActiveObject::started() {
    
}

void ActiveObject::stopped() {
    
}

void ActiveObject::setStoppedFunction(boost::function<void(ActiveObjectPtr)> fn) {
    stop_function = fn;
}

void ActiveObject::setStartedFunction(boost::function<void(ActiveObjectPtr)> fn) {
    std::cout <<  this  <<" Setting start function" << std::endl;
    start_function= fn;
}

void ActiveObject::run() {
    std::cout <<  this  <<" Starting active object ... Run called" << std::endl;
    service.reset( new boost::asio::io_service());
    started();
    if(start_function) {
        std::cout <<  this  <<" Starting active object ... calling start_function" << std::endl;
        start_function(shared_from_this());
    } else  {
        std::cout <<  this  <<" Start function unset ..." << std::endl;
    }
    worker.reset(new boost::asio::io_service::work(*service));
    service->post(boost::bind(&ActiveObject::startPool, this));
    service->run();
    if(stop_function)
        stop_function(shared_from_this());
    stopped();
    for(auto thread: threads)
        thread->interrupt();
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
