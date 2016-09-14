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
#include <tests/common/contexts/some_context.h>
#include <tests/common/actions/some_actions.h>

/**
 Test 4 is a Producer consummer test. It ensure that an action can produce an output, and another can expect an input to be provided.
 
 */


void TestClient::prepareTest() {
    BOOST_LOG_SEV(logger, Info) << logActor() << "Adding workflow";
    WorkflowPtr workflow(new Workflow("test-4"));
    auto sm = workflow->getStateMachine();
    sm->addAction(0, new SomeProducer(), {
        OutputBinding(0, "some_data", 1, "some_data")
    });
    sm->addAction(1, new SomeConsummer(), {
        OutputBinding(1, "", (int32_t)Step::Finish, "")
    });
    sm->addInput(InputBinding("", 0, ""));
    
    // registering it.
    ControllerManager::getInstance()->getController("default")->addWorkflow(workflow);
    
    RequestPtr request(new Request(Target("test-4"), Target("test_result")));
    request->getTarget().workflow = "test-4";
        
    expect(request, ETestResult::Success);
    publishRequest(request);
    
    
    std::string str;
    workflow->str_save(str);
    std::ofstream fs;
    fs.open("jsons/test_4.json");
    fs << str;
    fs.flush();
    fs.close();
}