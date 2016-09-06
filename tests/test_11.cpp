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
 Test 10 is a Workflow input Test. This is a simple A workflow, where A allows a context input. We provide another context context.
 */


void TestClient::prepareTest() {
    BOOST_LOG_SEV(logger, Info) << logActor() << "Adding workflow";
    WorkflowPtr workflow(new Workflow("test-11"));
    auto sm = workflow->getStateMachine();
    sm->addAction(0, new SomeAllowed(), {
        OutputBinding(0, "", (int32_t)Step::Finish, "")
    });
    sm->addInput(InputBinding("some_data", 0, "some_data"));
    
    // registering it.
    ControllerManager::getInstance()->getController("default")->addWorkflow(workflow);
    
    RequestPtr request(new Request(Target("test-11"), Target("test_result")));
    request->getTarget().workflow = "test-11";
    
    auto ctx = new Context();
    request->setContext("some_data", ctx);
    
    expect(request, ETestResult::Failure);
    publishRequest(request);
}


/*
 // Workflow creation
 // It's a pretty simple workflow that does absolutely nothing :)
 // Aside printing hello
 
 */