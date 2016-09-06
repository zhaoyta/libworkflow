#include <iostream>
#include <core/workflow.h>
#include <core/action.h>
#include <core/state_machine.h>
#include <core/bindings.h>
#include <tests/common/test_client.h>
#include <tests/common/actions/do_next.h>
#include <tests/common/contexts/some_context.h>
#include <tests/common/actions/some_actions.h>
#include <service/actor.h>
#include <service/controller_manager.h>
#include <core/controller.h>

/**
 Test 6 is a simple A->B workflow where B expect some context but doens't get it.
 */


void TestClient::prepareTest() {
    BOOST_LOG_SEV(logger, Info) << logActor() << "Adding workflow";
    WorkflowPtr workflow(new Workflow("test-6"));
    auto sm = workflow->getStateMachine();
    sm->addAction(0, new DoNext(), {
        OutputBinding(0, "", 1, "")
    });
    sm->addAction(1, new SomeConsummer(), {
        OutputBinding(1, "", 2, "")
    });
    sm->addAction(2, new SomeProducer(), {
        OutputBinding(2, "some_data", (int32_t)Step::Finish, "some_data")
    });
    sm->addInput(InputBinding("", 0, ""));
    
    // registering it.
    ControllerManager::getInstance()->getController("default")->addWorkflow(workflow);
    
    RequestPtr request(new Request(Target("test-6"), Target("test_result")));
    request->getTarget().workflow = "test-6";
        
    expect(request, ETestResult::Failure);
    publishRequest(request);
}


/*
 // Workflow creation
 // It's a pretty simple workflow that does absolutely nothing :)
 // Aside printing hello
 
*/