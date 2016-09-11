#include <tests/common/test_client.h>
#include <service/controller_manager.h>
#include <core/controller.h>
#include <tools/timed.h>
#include <tools/logged.h>
#include <service/client_manager.h>
#include <core/request.h>

#include <tools/action_factory.h>
#include <tools/context_factory.h>

#include <tests/common/actions/some_actions.h>
#include <tests/common/actions/do_next.h>
#include <tests/common/actions/error.h>
#include <tests/common/actions/print_log.h>

#include <tests/common/contexts/some_context.h>


BEGIN_ENUM_IMPL(TestResult) {
    {"Succeed",(uint32_t)ETestResult::Success},
    {"Failed",(uint32_t)ETestResult::Failure},
    {"Partial",(uint32_t)ETestResult::Partial},
    {"None",(uint32_t)ETestResult::None},
};
END_ENUM_IMPL(TestResult);


//! Quickly interrupt the service.
void terminateTest();

static int main_return_value = 0;

boost::shared_ptr<boost::asio::io_service::work> work;
boost::shared_ptr<boost::asio::io_service> service;
TimedPtr t;

TestClientPtr test_client;


TestClient::TestClient():
    Actor("TestClient"),
Logged("test.log"), global_result(ETestResult::None) {
    
}

TestClient::~TestClient() {
    
}


void TestClient::started() {
    connect();
    
    prepareTest();
    
}

void TestClient::newRequestReceived() {
    auto req = dequeuePendingRequest();
    ETestResult result = ETestResult::Failure;
    if(req) {
        if(req->getTarget().target == ETargetAction::Reply){
            result = ETestResult::Success;
        }
    } else {
        // No request :)
        return;
    }
    
    if(expected_results.count(req->getRequestId()) > 0 ) {
        if(expected_results[req->getRequestId()] == result) {
            if(additionnal_data.count(req->getRequestId())> 0) {
                ETestResult res;
                boost::function<void(ActorPtr,RequestPtr,ETestResult&)> fn = additionnal_data[req->getRequestId()];
                fn(shared_from_this(), req, res);
                if(res == ETestResult::Success) {
                    
                    BOOST_LOG_SEV(logger, Info) << logActor() << req->logRequest() << " met expected results";
                    
                    
                    if(global_result == ETestResult::None)
                        global_result = ETestResult::Success;
                    else if(global_result != ETestResult::Success)
                        global_result = ETestResult::Partial;
                    
                } else {
                    
                    BOOST_LOG_SEV(logger, Error) << logActor() << req->getReply() << " failed to meet expected results";
                    if(global_result == ETestResult::None)
                        global_result = ETestResult::Failure;
                    else if(global_result == ETestResult::Success)
                        global_result = ETestResult::Partial;
                }
            } else {
            BOOST_LOG_SEV(logger, Info) << logActor() << req->logRequest() << " met expected results";
                
                if(global_result == ETestResult::None)
                    global_result = ETestResult::Success;
                else if(global_result != ETestResult::Success)
                    global_result = ETestResult::Partial;
            }
        } else {
            BOOST_LOG_SEV(logger, Error) << logActor() << req->getReply() << " failed to meet expected results";
            if(global_result == ETestResult::None)
                global_result = ETestResult::Failure;
            else if(global_result == ETestResult::Success)
                global_result = ETestResult::Partial;
        }
        expected_results.erase(req->getRequestId());
    }
    
    if(expected_results.size() == 0) {
        if(global_result == ETestResult::Success) {
            BOOST_LOG_SEV(logger, Info) << " Test Succeeded !";
            main_return_value = 0;
        } else {
            main_return_value = 1;
            BOOST_LOG_SEV(logger, Error) << " Test Failed !";
        }
        
        ::service->post(&terminateTest);
    }
}



void TestClient::expect(RequestPtr req, ETestResult expected_res) {
    expected_results[req->getRequestId()] = expected_res;
    
}

void TestClient::expect(RequestPtr req, ETestResult expected_res, boost::function<void(ActorPtr,RequestPtr,ETestResult&)> funct) {
    
    expected_results[req->getRequestId()] = expected_res;
    additionnal_data[req->getRequestId()] = funct;
    
}

void TestClient::schedule(RequestPtr req, double ms_timeout) {
    scheduled_requests[ms_timeout].push_back(req);
}

void TestClient::startScheduler() {
    begin.reset();
    nextStop.reset();
    nextStop.setIOService(getIOService());
    nextStop.setTimeoutFunction(boost::bind(&TestClient::startNextBatch, this));
    
    scheduleNextBatch();
}

void TestClient::startNextBatch() {
    std::vector<double> toremove;
    for(const auto & kv: scheduled_requests) {
        auto nexts = kv.second;
        auto lapse = kv.first;
        if(lapse - begin.elapsed() <= 0) {
            for(auto r: nexts) {
                publishRequest(r);
            }
            toremove.push_back(lapse);
        } else {
            break;
        }
    }
    
    for(const auto & d: toremove)
        scheduled_requests.erase(d);
    scheduleNextBatch();
}

void TestClient::scheduleNextBatch() {
    if(scheduled_requests.size() > 0 ) {
        auto nexts = scheduled_requests.begin()->second;
        auto lapse = scheduled_requests.begin()->first;
        
        nextStop.setDuration(lapse - begin.elapsed());
        nextStop.start();
        
    } else {
        BOOST_LOG_SEV(logger, Info) << logActor() << " Nothing more to send !";
    }
}




//! interrupt the service by TimeOut
void terminateTO(const boost::system::error_code & ec);

void delayedTermination(const boost::system::error_code & ec ) {
    work.reset();
}

void terminateTest() {
    if(t) {
        t->stop();
        t.reset();
    }
    
    GLOB_LOGGER("general");
    BOOST_LOG_SEV(logger, Info) << " Shutting down !";
    ControllerManager::getInstance()->terminate();
    ClientManager::getInstance()->terminate();
    
    
    t.reset(new Timed());
    t->setIOService(service);
    t->setDuration(1000);
    t->setTimeoutFunction(&delayedTermination);
    t->start();
}


void terminateTO(const boost::system::error_code & ec ) {
    if(not ec) {
        terminateTest();
    } else {
        GLOB_LOGGER("general");
        BOOST_LOG_SEV(logger, Info) << " Unexpected call Oo ! " << ec.message();
    }
}
void setGlobalTimeout(double ms) {
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
    
    test_client.reset(new TestClient());
    ClientManager::getInstance()->addClient(test_client);
    
    BOOST_LOG_SEV(logger, Info) << " Load JSON-ready Action and Contexts... ";
    ActionFactory::registerAction(new ActionBuilder<SomeProducer>());
    ActionFactory::registerAction(new ActionBuilder<SomeConsummer>());
    ActionFactory::registerAction(new ActionBuilder<SomeAllowed>());
    ActionFactory::registerAction(new ActionBuilder<ErrorAction>());
    ActionFactory::registerAction(new ActionBuilder<DoNext>());
    ActionFactory::registerAction(new ActionBuilder<PrintLog>());
    ContextFactory::registerContext(new ContextBuilder<SomeContext>());
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
    
    return main_return_value;
}


