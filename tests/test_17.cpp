#include <iostream>
#include <core/workflow.h>
#include <core/action.h>
#include <core/state_machine.h>
#include <core/bindings.h>
#include <tests/common/test_client.h>
#include <service/actor.h>
#include <service/controller_manager.h>
#include <core/controller.h>
#include <core/put_definition.h>
#include <tools/error_report.h>

#include <fstream>
#include <streambuf>

/**
    Test 17: Load a workflow from JSON.
 */
void TestClient::prepareTest() {
    RequestPtr request(new Request(Target("test-2"), Target("test_result")));


    std::string json;
    
    {
        std::ifstream t("jsons/test_2.json");
        
        t.seekg(0, std::ios::end);
        json.reserve(t.tellg());
        t.seekg(0, std::ios::beg);
        
        json.assign((std::istreambuf_iterator<char>(t)),
                   std::istreambuf_iterator<char>());
    }
    
    WorkflowPtr wkf(new Workflow("loading..."));
    wkf->str_load(json);
    auto err = wkf->getStateMachine()->validate();
    if(err.isSet()) {
        // Abort the job by error.
        BOOST_LOG_SEV(logger, Error) << " " << err.getErrorMessage();
        abortTest();
    } else {
        ControllerManager::getInstance()->getController()->addWorkflow(wkf);
        expect(request, ETestResult::Success);
        publishRequest(request);
    }
}

