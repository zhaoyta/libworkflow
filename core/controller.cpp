#include <core/controller.h>
#include <core/workflow.h>
#include <core/controller_spawn.h>


Controller::Controller(const std::string & name) :
    ActiveObject(name),
    boost::enable_shared_from_this<Controller>() {}
Controller::~Controller() {}

bool Controller::perform(RequestPtr request) {
    if(workflows.count(request->getTarget().workflow) > 0 ) {
        request->setControllerSpawn(spawnForRequest(request));
        return workflows[request->getTarget().workflow]->perform(request);
    }
    return false;
}

ControllerSpawnPtr Controller::spawnForRequest(RequestPtr) {
    return ControllerSpawnPtr(new ControllerSpawn());
}

const std::string & Controller::getName() {
    return name;
}

void Controller::addWorkflow(WorkflowPtr workflow) {
    workflow->setController(shared_from_this());
    workflows[workflow->getName()] = workflow;
}