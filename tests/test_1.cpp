#include <iostream>
#include <core/workflow.h>
#include <core/action.h>
#include <core/state_machine.h>
#include <core/bindings.h>
#include <tests/common/test_client.h>
#include <tests/common/actions/do_next.h>
#include <service/actor.h>
#include <service/controller_manager.h>
#include <core/controller.h>

/**
 @file This test simply ensure that the basic stack is up and running. It means that if THIS test fails, all other will as well. 
    More to the point, it ensure a Workflow can be added, a request can be issued and replied to, a workflow can be reached and executed, and basic test components works
 
 As a note, you should be made aware that a TestClient has been prepared in commons/ directory.
 Later some commons Actions will be added but also some workflow json files as well.
 */


/**
    This should be the simplest of test.
    Create a new workflow with one Action. 
    Execute it and that's it.
 */


void TestClient::prepareTest() {
    BOOST_LOG_SEV(logger, Info) << logActor() << "Adding workflow";
    WorkflowPtr workflow(new Workflow("test-1"));
    auto sm = workflow->getStateMachine();
    sm->addAction(0, new DoNext(), {
        OutputBinding(0, "", (int32_t)Step::Finish, "")
    });
    sm->addInput(InputBinding("", 0, ""));
    
    // registering it.
    ControllerManager::getInstance()->getController("default")->addWorkflow(workflow);
    
    RequestPtr request(new Request(Target("test-1"), Target("test_result")));
    request->getTarget().workflow = "test-1";
        
    expect(request, ETestResult::Success);
    publishRequest(request);
}


/*
 // Workflow creation
 // It's a pretty simple workflow that does absolutely nothing :)
 // Aside printing hello
 
*/