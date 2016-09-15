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
 Test 18: Prepare a nice request from json, where a workflow has been provided.
 */
void TestClient::prepareTest() {
    RequestPtr request(new Request());
    std::string json;
    
    {
        std::ifstream t("jsons/request_2.json");
        
        t.seekg(0, std::ios::end);
        json.reserve(t.tellg());
        t.seekg(0, std::ios::beg);
        
        json.assign((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    }
    
    request->str_load(json);
    
    expect(request, ETestResult::Success);
    publishRequest(request);
}

