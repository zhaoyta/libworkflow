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
 Test 20
 */
void TestClient::prepareTest() {
    RequestPtr request(new Request());
    
    expect(request, ETestResult::Success);
    publishRequest(request);
}

