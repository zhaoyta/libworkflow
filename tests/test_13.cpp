#include <iostream>
#include <core/workflow.h>
#include <core/action.h>
#include <core/state_machine.h>
#include <core/bindings.h>
#include <tests/common/test_client.h>
#include <tests/common/actions/do_next.h>
#include <tests/common/actions/print_log.h>
#include <tests/common/actions/error.h>
#include <tests/common/contexts/some_context.h>
#include <tests/common/actions/some_actions.h>
#include <service/actor.h>
#include <service/controller_manager.h>
#include <core/controller.h>
#include <core/put_definition.h>

/**
 Test 13 Split executiong workflow:
            A
           / \
          B   C
 
    This workflow will execute both B and C from A.
 */

void TestClient::prepareTest() {
    BOOST_LOG_SEV(logger, Info) << logActor() << "Adding workflow";
    WorkflowPtr workflow(new Workflow("test-13"));
    auto sm = workflow->getStateMachine();
    sm->addAction(0, new DoNext, {
        OutputBinding(0,"", 1, ""),
        OutputBinding(0,"", 2, "")
    });
    sm->addAction(1, new PrintLog(), {
        OutputBinding("", Step::Finish, "")
    });
    sm->addAction(2, new PrintLog(), {
        OutputBinding("", Step::Finish, "")
    });
    sm->addInput(InputBinding("", 0, ""));
    
    // registering it.
    ControllerManager::getInstance()->getController("default")->addWorkflow(workflow);
    
    RequestPtr request(new Request(Target("test-13"), Target("test_result")));
    request->getTarget().workflow = "test-13";
    
    expect(request, ETestResult::Success);
    publishRequest(request);
}

