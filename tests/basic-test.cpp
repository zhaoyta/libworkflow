#include <iostream>
#include <core/workflow.h>
#include <core/action.h>
#include <core/state_machine.h>
#include <core/bindings.h>
#include <service/controller_manager.h>
#include <core/controller.h>
#include <tools/timed.h>
#include <tools/logged.h>

/**
    This should be the simplest of test.
    Create a new workflow with one Action. 
    Execute it and that's it.
 */


class TestActionA: public Action {
public:
    TestActionA() : Action("TestActionA") {}
    
    Result perform(SessionPtr session) const override{
        BOOST_LOG_SEV(logger, Info) ;
        // fingerprint(session) << " Hello from Action Test A";
        return done();
    }
    
};
        
boost::shared_ptr<boost::asio::io_service::work> work;
boost::shared_ptr<boost::asio::io_service> service;
TimedPtr t;
        
void terminate(const boost::system::error_code & ec ) {
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
        
        
void delayed(ActiveObjectPtr) {
    
    GLOB_LOGGER("general");

    BOOST_LOG_SEV(logger, Info) << " Setting up Tests workflow !";
    // timeout execution.
    t.reset(new Timed());
    t->setIOService(service);
    t->setDuration(5000);
    t->setTimeoutFunction(&terminate);
    t->start();
    
    WorkflowPtr workflow(new Workflow("test-workflow-a"));
    auto sm = workflow->getStateMachine();
    sm->addAction(0, new TestActionA(), {
        OutputBinding(0, "", (int32_t)Step::Finish, "")
    });
    RequestPtr request(new Request(Target("test-workflow-a")));
    request->getTarget().workflow = "test-workflow-a";
    
    
    ControllerManager::getInstance()->getController("default")->addWorkflow(workflow);
    
    workflow->perform(request);
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
