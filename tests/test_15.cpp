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
  Test 15 is a Loop Test.
 
    A ---- Finish
   / \
  B - C
 
   This A will be executed twice. The first time, Loop output will be allowed and Finish will be restricted.
   The second time, Finish will be allowed and Loop will be restricted.
 */

class LoopConditionTestAction: public Action {
public:
    LoopConditionTestAction(): Action("LoopConditionTestAction") {
        PutDefinition ploop;
        ploop.put_name = "Loop";
        ploop.allowSkip = true;
        ploop.checker.reset(CHECKER(Context));
        defineOutput(ploop);
        PutDefinition p;
        p.put_name = "Finish";
        p.mandatory = false;
        p.allowSkip = true;
        p.checker.reset(CHECKER(Context));
        defineOutput(p);
        
        properties()->defineBoolProperty("should_loop", true, "Allow looping. or not.");
    }
    
    Result perform(SessionPtr session) const override {
        bool shouldLoop = boolProperty(session,"should_loop");
        session->getBypass(getActionId())->setBoolProperty("should_loop", false);
        if(shouldLoop) {
            setOutput(session, "Loop", new Context());
            setOutput(session, "Finish", new SkipCtx());
        } else  {
            setOutput(session, "Loop", new SkipCtx());
            setOutput(session, "Finish", new Context());
        }
        return done();
    }
};

void TestClient::prepareTest() {
    BOOST_LOG_SEV(logger, Info) << logActor() << "Adding workflow";
    WorkflowPtr workflow(new Workflow("test-15"));
    auto sm = workflow->getStateMachine();
    sm->addAction(0, new LoopConditionTestAction(), {
        OutputBinding(0,"Loop", 1, ""),
        OutputBinding(0,"Finish", Step::Finish, "")
    });
    sm->addAction(1, new PrintLog(), {
        OutputBinding(1, "", 2, "")
    });
    sm->addAction(2, new DoNext(), {
        OutputBinding(2, "", 0, "")
    });
    sm->addInput(InputBinding("", 0, ""));
    
    // registering it.
    ControllerManager::getInstance()->getController("default")->addWorkflow(workflow);
    
    RequestPtr request(new Request(Target("test-15"), Target("test_result")));
    request->getTarget().workflow = "test-15";
    
    expect(request, ETestResult::Success);
    publishRequest(request);
}

