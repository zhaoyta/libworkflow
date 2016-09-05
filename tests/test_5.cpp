#include <iostream>
#include <core/workflow.h>
#include <core/action.h>
#include <core/state_machine.h>
#include <core/bindings.h>
#include <tests/common/test_client.h>
#include <tests/common/actions/print_log.h>
#include <tests/common/contexts/some_context.h>
#include <service/actor.h>
#include <service/controller_manager.h>
#include <core/controller.h>

/**
 This is a simple A->B Workflow where A should produce some context but doesn't.
 */

class FailProducerAction: public Action {
public:
    FailProducerAction(): Action("FailProducerAction") {
        defineOutput("some_data", CHECKER(SomeContext));
    }
    Result perform(SessionPtr session) const override {
        BOOST_LOG_SEV(logger, Info) << fingerprint(session) << " Failing to produce expected context!";
        return done();
    }
};

void TestClient::prepareTest() {
    BOOST_LOG_SEV(logger, Info) << logActor() << "Adding workflow";
    WorkflowPtr workflow(new Workflow("test-5"));
    auto sm = workflow->getStateMachine();
    sm->addAction(0, new FailProducerAction(), {
        OutputBinding(0, "", 1, "") // bind it to next
    });
    sm->addAction(1, new PrintLog(), {
        OutputBinding(1, "", (int32_t)Step::Finish, "")
    });
    sm->addInput(InputBinding("", 0, ""));
    
    // registering it.
    ControllerManager::getInstance()->getController("default")->addWorkflow(workflow);
    
    RequestPtr request(new Request(Target("test-5"), Target("test_result")));
    request->getTarget().workflow = "test-5";
        
    expect(request, ETestResult::Failure);
    publishRequest(request);
}
