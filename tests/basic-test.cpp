#include <iostream>
#include <core/workflow.h>
#include <core/action.h>
#include <core/state_machine.h>
#include <core/bindings.h>


/**
    This should be the simplest of test.
    Create a new workflow with one Action. 
    Execute it and that's it.
 */

class TestActionA: public Action {
public:
    TestActionA() : Action("TestActionA") {}
    
    Result perform(SessionPtr session) const override{
        std::cout << " Test Action A";
        return done();
    }
    
};

int main(int argc, const char * argv[]) {
    WorkflowPtr workflow(new Workflow("test-workflow-a"));
    auto sm = workflow->getStateMachine();
    sm->addAction(0, new TestActionA(), {
        OutputBinding(0, "", (int32_t)Step::Finish, "")
    });
    RequestPtr request(new Request());
    request->getTarget().workflow = "test-workflow-a";
    
    bool done = workflow->perform(request);
    
    std::cout << " Done ? : " << done;
    
    return 0;
}
