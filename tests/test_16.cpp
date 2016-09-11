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
 Test 16 ensure an action can itself override global property. (Test 15 ensure one action can override action specific property. )
 */

class SetProperty: public Action {
public:
    SetProperty(): Action("SetProperty") {
    }
    
    Result perform(SessionPtr session) const override {
        session->getBypass()->setStringProperty("log", "Some Custom Log");
        return done();
    }
};

void TestClient::prepareTest() {
    BOOST_LOG_SEV(logger, Info) << logActor() << "Adding workflow";
    WorkflowPtr workflow(new Workflow("test-16"));
    auto sm = workflow->getStateMachine();
    sm->addAction(0, new SetProperty(), {
        OutputBinding(0,"", 1, ""),
    });
    sm->addAction(1, new PrintLog(), {
        OutputBinding(1, "", Step::Finish, "")
    });
    sm->addInput(InputBinding("", 0, ""));
    
    // registering it.
    ControllerManager::getInstance()->getController("default")->addWorkflow(workflow);
    
    RequestPtr request(new Request(Target("test-16"), Target("test_result")));
    request->getTarget().workflow = "test-16";
    
    expect(request, ETestResult::Success);
    publishRequest(request);
    
    
    std::string str;
    workflow->str_save(str);
    std::ofstream fs;
    fs.open("test_16.json");
    fs << str;
    fs.flush();
    fs.close();
}

