#include <core/controller.h>
#include <core/workflow.h>
#include <core/controller_spawn.h>
#include <core/request.h>
#include <core/target.h>


Controller::Controller(const std::string & name, uint32_t pool) :
    ActiveObject(name, pool) {}
Controller::~Controller() {}

bool Controller::perform(RequestPtr request) {
    if(workflows.count(request->getTarget().workflow) > 0 ) {
        request->setControllerSpawn(spawnForRequest(request));
        return workflows[request->getTarget().workflow]->perform(request);
    }
    return false;
}

ControllerPtr Controller::shared_from_this() {
    return boost::dynamic_pointer_cast<Controller>(ActiveObject::shared_from_this());
}

ControllerSpawnPtr Controller::spawnForRequest(RequestPtr) {
    return ControllerSpawnPtr(new ControllerSpawn());
}

void Controller::addWorkflow(WorkflowPtr workflow) {
    workflow->setController(shared_from_this());
    workflows[workflow->getName()] = workflow;
}