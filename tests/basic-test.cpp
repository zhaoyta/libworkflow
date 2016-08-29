#include <iostream>
#include <core/workflow.h>
#include <core/action.h>
#include <core/state_machine.h>
#include <core/bindings.h>
#include <service/controller_manager.h>
#include <core/controller.h>
#include <tools/timed.h>

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
        
        
void terminate(const boost::system::error_code & ) {
    std::cout << " Killing main controller" << std::endl;
    ControllerManager::getInstance()->terminate();
    work.reset();
}
        
        
void delayed(ActiveObjectPtr) {
    // timeout execution.
    Timed t;
    t.setIOService(ControllerManager::getInstance()->getIOService());
    t.setDuration(1000);
    t.setTimeoutFunction(&terminate);
    t.start();
    
    std::cout << " Loading main controller" << std::endl;
    
    WorkflowPtr workflow(new Workflow("test-workflow-a"));
    auto sm = workflow->getStateMachine();
    sm->addAction(0, new TestActionA(), {
        OutputBinding(0, "", (int32_t)Step::Finish, "")
    });
    RequestPtr request(new Request());
    request->getTarget().workflow = "test-workflow-a";
    
    
    ControllerManager::getInstance()->getController("default")->addWorkflow(workflow);
    
    bool done = workflow->perform(request);
    
    std::cout << " Done ? : " << done;
}

        
int main(int argc, const char * argv[]) {
    // maybe we should wait for controller to have started ...
    std::cout << " Start" << std::endl;
    ControllerManager::getInstance()->setStartedFunction(&delayed);
    std::cout << " Starting main controller" << std::endl;
    ControllerManager::getInstance()->start();
    
    
    // keep alive.
    boost::asio::io_service service;
    work.reset(new boost::asio::io_service::work(service));
    service.run();
    
    return 0;
}
