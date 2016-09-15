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
 Test 19: Start a new Workflow, and provide a context as well, the whole from a simple request.
 */
void TestClient::prepareTest() {
    RequestPtr request(new Request());
    std::string json;
    
    {
        std::ifstream t("jsons/request_7.json");
        
        t.seekg(0, std::ios::end);
        json.reserve(t.tellg());
        t.seekg(0, std::ios::beg);
        
        json.assign((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    }
    
    request->str_load(json);
    
    expect(request, ETestResult::Success);
    publishRequest(request);}

