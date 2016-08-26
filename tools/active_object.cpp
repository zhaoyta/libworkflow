#include <tools/active_object.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/interprocess/interprocess_fwd.hpp>

ActiveObject::ActiveObject(const std::string &name, bool delay_start):
    boost::enable_shared_from_this<ActiveObject>(),
    name(name),
    mutex(new boost::recursive_mutex())
{
  if(not delay_start)
      start();
}

ActiveObject::~ActiveObject() {
    
}

void ActiveObject::start() {
    boost::mutex::scoped_lock<boost::recursive_mutex> sl(*mutex);
    if( not thread ) {
        thread.reset(new boost::thread(&ActiveObject::run, this));
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
    service->run();
    stopped();
    thread->stop();
}

OSTREAM_HELPER_IMPL(ActiveObject, obj) {
    out << "[ActiveObject] name: " << obj.getName();
    return out;
}
