#include <iostream>
#include <core/workflow.h>
#include <core/action.h>
#include <core/state_machine.h>
#include <core/bindings.h>
#include <tests/common/test_client.h>
#include <tests/common/actions/do_next.h>
#include <tests/common/actions/print_log.h>
#include <service/actor.h>
#include <service/controller_manager.h>
#include <core/controller.h>
#include <tools/property_set.h>



/**
 
 Test 2 is a simple A->B->C workflow. 
 B will use a property "log" as string used to display some info.
 C will use the same property, but will be overriden by an action specific override. 
 
 The whole result in a succes workflow.
 
 */
void TestClient::prepareTest() {
    BOOST_LOG_SEV(logger, Info) << logActor() << "Adding workflow";
    WorkflowPtr workflow(new Workflow("test-2"));
    auto sm = workflow->getStateMachine();
    sm->addAction(0, new DoNext(), {
        OutputBinding(0, "", 1, "")
    });
    sm->addAction(1, new PrintLog(), {
        OutputBinding(2, "", 2, "")
    });
    sm->addAction(2, new PrintLog(), {
        OutputBinding(2, "", (int32_t)Step::Finish, "")
    });
    sm->addInput(InputBinding("", 0, ""));
    
    // registering it.
    ControllerManager::getInstance()->getController("default")->addWorkflow(workflow);
    
    RequestPtr request(new Request(Target("default", "test-2"), Target("test_result")));
    request->getTarget().workflow = "test-2";
    request->getBypass()->setStringProperty("log", "Some General Log");
    request->getActionBypasses(2)->setStringProperty("log", "Some Action Specific Log");
        
    expect(request, ETestResult::Success);
    publishRequest(request);
    
    std::string str;

    workflow->str_save(str);
    {
    std::ofstream fs;
    fs.open("jsons/test_2.json");
    fs << str;
    fs.flush();
    fs.close();
    }
    
    request->setWorkflowJson(str);
    request->str_save(str);
    {
        std::ofstream fs;
        fs.open("jsons/request_2.json");
        fs << str;
        fs.flush();
        fs.close();
    }
    request->setWorkflowJson("");
    
}


/*
 // Workflow creation
 // It's a pretty simple workflow that does absolutely nothing :)
 // Aside printing hello
 
*/