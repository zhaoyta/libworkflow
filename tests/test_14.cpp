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
 Test 14 Condition workflow:
   A
  / \
 B ? C
 
 This workflow will execute B over C.
 */

class ConditionTestAction: public Action {
public:
    ConditionTestAction(): Action("ConditionTestAction") {
        defineOutput("Yes", CHECKER(Context));
        PutDefinition p;
        p.put_name = "No";
        p.allowSkip = true;
        p.checker.reset(CHECKER(Context));
        defineOutput(p);
    }
    
    Result perform(SessionPtr session) const override {
        setOutput(session, "Yes", new Context());
        setOutput(session, "No", new SkipCtx()); // this ensure that whatever is connected to this output never gets executed.
        return done();
    }
};

void TestClient::prepareTest() {
    BOOST_LOG_SEV(logger, Info) << logActor() << "Adding workflow";
    WorkflowPtr workflow(new Workflow("test-14"));
    auto sm = workflow->getStateMachine();
    sm->addAction(0, new ConditionTestAction(), {
        OutputBinding(0,"Yes", 1, ""),
        OutputBinding(0,"No", 2, "")
    });
    sm->addAction(1, new PrintLog(), {
        OutputBinding(1, "", Step::Finish, "")
    });
    sm->addAction(2, new ErrorAction(), {
        OutputBinding(2, "", Step::Finish, "")
    });
    sm->addInput(InputBinding("", 0, ""));
    
    // registering it.
    ControllerManager::getInstance()->getController("default")->addWorkflow(workflow);
    
    RequestPtr request(new Request(Target("test-14"), Target("test_result")));
    request->getTarget().workflow = "test-14";
    
    expect(request, ETestResult::Success);
    publishRequest(request);
}

