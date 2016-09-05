#include <iostream>
#include <core/workflow.h>
#include <core/action.h>
#include <core/state_machine.h>
#include <core/bindings.h>
#include <tests/common/test_client.h>
#include <tests/common/actions/do_next.h>
#include <tests/common/actions/print_log.h>
#include <tests/common/actions/error.h>
#include <service/actor.h>
#include <service/controller_manager.h>
#include <core/controller.h>

/**
    This is a A->B-xC workflow, C should print a never displayed message, because B should return an error.
 */


void TestClient::prepareTest() {
    BOOST_LOG_SEV(logger, Info) << logActor() << "Adding workflow";
    WorkflowPtr workflow(new Workflow("test-3"));
    auto sm = workflow->getStateMachine();
    sm->addAction(0, new DoNext(), {
        OutputBinding(0, "", 1, "")
    });
    sm->addAction(1, new ErrorAction(), {
        OutputBinding(1, "", 2, "")
    });
    sm->addAction(2, new PrintLog(), {
        OutputBinding(2, "", (int32_t)Step::Finish, "")
    });
    sm->addInput(InputBinding("", 0, ""));
    
    // registering it.
    ControllerManager::getInstance()->getController("default")->addWorkflow(workflow);
    
    RequestPtr request(new Request(Target("test-3"), Target("test_result")));
    request->getTarget().workflow = "test-3";
        
    expect(request, ETestResult::Failure);
    publishRequest(request);
}


/*
 // Workflow creation
 // It's a pretty simple workflow that does absolutely nothing :)
 // Aside printing hello
 
*/