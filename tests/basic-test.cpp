#include <iostream>
#include <core/workflow.h>
#include <core/action.h>
#include <core/state_machine.h>
#include <core/bindings.h>
#include <tests/common/test_client.h>
#include <service/actor.h>
#include <service/controller_manager.h>
#include <core/controller.h>


/**
    This should be the simplest of test.
    Create a new workflow with one Action. 
    Execute it and that's it.
 */
class TestActionA: public Action {
public:
    TestActionA() : Action("TestActionA") {}
    
    Result perform(SessionPtr session) const override{
        BOOST_LOG_SEV(logger, Info) << fingerprint(session) << " Hello from Action Test A";
        return done();
    }
    
};



SHARED_PTR(TestClient);

class TestClient: public Actor {
public:
    TestClient(): Actor("TestClient") {
        setNamespace("test.log");
    }
    virtual ~TestClient(){}
    
protected:
    
    void started() override {
        WorkflowPtr workflow(new Workflow("test-workflow-a"));
        auto sm = workflow->getStateMachine();
        sm->addAction(0, new TestActionA(), {
            OutputBinding(0, "", (int32_t)Step::Finish, "")
        });
        sm->addInput(InputBinding("", 0, ""));
        
        // registering it.
        ControllerManager::getInstance()->getController("default")->addWorkflow(workflow);
        
        RequestPtr request(new Request(Target("test-workflow-a")));
        request->getTarget().workflow = "test-workflow-a";
        
        publishRequest(request);
    }
    
    void newRequestReceived() override {
        
    }
};

TestClientPtr test_client;


void test_main() {
    test_client.reset(new TestClient());
}

/*
 // Workflow creation
 // It's a pretty simple workflow that does absolutely nothing :)
 // Aside printing hello
 
*/