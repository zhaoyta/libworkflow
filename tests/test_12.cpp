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
 Test 12 is a test of workflow output. A produces some_data, we check we get it fair and square.
 */


void TestClient::prepareTest() {
    BOOST_LOG_SEV(logger, Info) << logActor() << "Adding workflow";
    WorkflowPtr workflow(new Workflow("test-12"));
    auto sm = workflow->getStateMachine();
    sm->addAction(0, new SomeProducer(), {
        OutputBinding("some_data", Step::Finish, "some_data")
    });
    sm->addInput(InputBinding("", 0, ""));
    
    // registering it.
    ControllerManager::getInstance()->getController("default")->addWorkflow(workflow);
    
    RequestPtr request(new Request(Target("test-12"), Target("test_result")));
    request->getTarget().workflow = "test-12";
        
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


/*
 // Workflow creation
 // It's a pretty simple workflow that does absolutely nothing :)
 // Aside printing hello
 
*/