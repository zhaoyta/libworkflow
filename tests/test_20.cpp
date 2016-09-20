#include <iostream>
#include <core/workflow.h>
#include <core/action.h>
#include <core/state_machine.h>
#include <core/bindings.h>
#include <tests/common/test_client.h>
#include <actions/communication/execute_workflow.h>
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
 Test 20: ExecuteWorkflow Test: have a workflow invoke another in a synchroneous manner. 
    Result of the other workflow should be forwarded to this one workflow output.
 */
void TestClient::prepareTest() {
    WorkflowPtr workflow(new Workflow("test-6"));
    auto sm = workflow->getStateMachine();
    sm->addAction(0, new DoNext(), {
        OutputBinding("", 1, "")
    });
    sm->addAction(1, new SomeProducer(), {
        OutputBinding("some_data", Step::Finish, "some_data")
    });
    sm->addInput(InputBinding("", 0, ""));
    
    // registering it.
    ControllerManager::getInstance()->getController("default")->addWorkflow(workflow);
    
    
    WorkflowPtr workflow20(new Workflow("test-20"));
    sm = workflow20->getStateMachine();
    sm->addAction(0, new DoNext(), {
        OutputBinding("", 1, "")
    });
    sm->addAction(1, new ExecuteWorkflow("default", "test-6"), {
        OutputBinding("some_data", (int32_t)Step::Finish, "some_data")
    });
    sm->addInput(InputBinding("", 0, ""));
    
    // registering it.
    ControllerManager::getInstance()->getController("default")->addWorkflow(workflow20);
    
    
    
    RequestPtr request(new Request(Target("test-20"), Target("test_result")));
    
    expect(request, ETestResult::Success, [&] (ActorPtr act,RequestPtr reply ,ETestResult& result) {
        if(reply->getContext("some_data")) {
            auto ctx = reply->getCastedContext<SomeContext>("some_data");
            if(ctx) {
                result = ETestResult::Success;
                return;
            }
        }
        
        result = ETestResult::Failure;
        return;
    });

    publishRequest(request);
}

