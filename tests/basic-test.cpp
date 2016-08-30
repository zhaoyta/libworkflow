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
        std::cout << " Test Action A" << std::endl;
        return done();
    }
    
};
        
boost::shared_ptr<boost::asio::io_service::work> work;
boost::shared_ptr<boost::asio::io_service> service;
        
void terminate(const boost::system::error_code & ) {
    ControllerManager::getInstance()->terminate();
    work.reset();
}
        
        
void delayed(ActiveObjectPtr) {
    // timeout execution.
    Timed t;
    t.setIOService(service);
    t.setDuration(1000);
    t.setTimeoutFunction(&terminate);
    t.start();
    
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
    ControllerManager::getInstance()->setStartedFunction(&delayed);
    ControllerManager::getInstance()->start();
    
    
    // keep alive.
    work.reset(new boost::asio::io_service::work(*service));
    service->run();
    
    return 0;
}
