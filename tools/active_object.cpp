#include <tools/active_object.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/interprocess/interprocess_fwd.hpp>

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

void ActiveObject::start() {
    boost::mutex::scoped_lock<boost::recursive_mutex> sl(*mutex);
    if( threads.size() == 0 ) {
        boost::shared_ptr<boost::thread> thread(new boost::thread(&ActiveObject::run, this));
        threads.push_back(thread);
    }
}

void ActiveObject::stop() {
    boost::mutex::scoped_lock<boost::recursive_mutex> sl(*mutex);
    worker->reset();
}

void ActiveObject::terminate() {
    boost::mutex::scoped_lock<boost::recursive_mutex> sl(*mutex);
    stop();
    thread->join();
}

void ActiveObject::started() {
    
}

void ActiveObject::stopped() {
    
}

void ActiveObject::run() {
    service.reset( new boost::asio::io_service());
    started();
    worker.reset(new )boost::asio::io_service::work(*service));
    service->post(&ActiveObject::startPool, this);
    service->run();
    stopped();
    thread->stop();
}

void ActiveObject::startPool() {
    while(threads.size() < thread_pool) {
        boost::shared_ptr<boost::thread> thread(new boost::thread(&boost::asio::io_service::run, *service)));
        threads.push_back(thread);
    }
}

OSTREAM_HELPER_IMPL(ActiveObject, obj) {
    out << "[ActiveObject] name: " << obj.getName();
    return out;
}
