#include <tests/common/test_client.h>
#include <service/controller_manager.h>
#include <core/controller.h>
#include <tools/timed.h>
#include <tools/logged.h>

boost::shared_ptr<boost::asio::io_service::work> work;
boost::shared_ptr<boost::asio::io_service> service;
TimedPtr t;

void terminate() {
    t->stop();
    t.reset();
    ControllerManager::getInstance()->terminate();
    work.reset();
}

void terminateTO(const boost::system::error_code & ec ) {
    if(not ec) {
        GLOB_LOGGER("general");
        BOOST_LOG_SEV(logger, Info) << " Shutting down !";
        ControllerManager::getInstance()->terminate();
        work.reset();
    } else {
        GLOB_LOGGER("general");
        BOOST_LOG_SEV(logger, Info) << " Unexpected call Oo ! " << ec.message();
    }
}
void setTimeout(double ms) {
    // timeout execution.
    t.reset(new Timed());
    t->setIOService(service);
    t->setDuration(ms);
    t->setTimeoutFunction(&terminateTO);
    t->start();
}

void delayed(ActiveObjectPtr) {
    GLOB_LOGGER("general");
    BOOST_LOG_SEV(logger, Info) << " Setting up Tests  !";    
    test_main();
}


int main(int argc, const char * argv[]) {
    // maybe we should wait for controller to have started ...
    service.reset(new boost::asio::io_service());
    Logged::loadConfiguration("");
    GLOB_LOGGER("general");
    BOOST_LOG_SEV(logger, Info) << " Test Starting !";
    ControllerManager::getInstance()->setStartedFunction(&delayed);
    ControllerManager::getInstance()->start();
    
    
    // keep alive.
    work.reset(new boost::asio::io_service::work(*service));
    service->run();
    
    return 0;
}
