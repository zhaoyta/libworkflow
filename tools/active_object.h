#ifndef __ACTIVE_OBJECT_H_
#define __ACTIVE_OBJECT_H_


#include <tools/defines.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <tools/logged.h>


#pragma GCC visibility push(default)

SHARED_PTR(ActiveObject);
typedef boost::shared_ptr<boost::asio::io_service> IOServicePtr;
namespace boost {
    class thread;
    class recursive_mutex;
};

/**
 ActiveObject is thread given a life time. 
 By default, it starts on construction, but can be delayed. in this case, the  active object expect to be started using start() function.
 
 When calling, from any thread, stop(), all active request planned will finish and then the object itself will stop.
 
 Thus, expect started() to be called from the thread once it's well started.
 stopped() will be called on this thread as well just before it's closure.
 */
class ActiveObject : public boost::enable_shared_from_this<ActiveObject>, public Logged {
    const std::string name;
    IOServicePtr service;
    std::vector< boost::shared_ptr<boost::thread> > threads;
    boost::shared_ptr<boost::asio::io_service::work> worker;
    boost::shared_ptr<boost::recursive_mutex> mutex;
    
    boost::function<void(ActiveObjectPtr)> start_function;
    boost::function<void(ActiveObjectPtr)> stop_function;
    
    uint32_t thread_pool;
public:
    ActiveObject(const std::string &name, uint32_t thread_pool = 1, bool delay_start = false);
    ActiveObject(const ActiveObject &) = delete; // disallow copy.
    virtual ~ActiveObject();
    
    void setStoppedFunction(boost::function<void(ActiveObjectPtr)>);
    void setStartedFunction(boost::function<void(ActiveObjectPtr)>);
    
    //! if not already running, will start the thread and io_service.
    void start();
    
    //! async stops the thread.
    void stop();
    
    //! sync stop the thread.
    void terminate();
    
    IOServicePtr getIOService();
    
    const std::string & getName() const;
    
    size_t getPoolSize() const;
    
protected:
    virtual void started();
    virtual void stopped();

private:
    void run();
    void startPool();
};

OSTREAM_HELPER_DECL(ActiveObject);


#pragma GCC visibility pop

#endif // __ACTIVE_OBJECT_H_