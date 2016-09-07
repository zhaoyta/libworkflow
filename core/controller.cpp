#include <core/controller.h>
#include <core/workflow.h>
#include <core/controller_spawn.h>
#include <core/state_machine.h>
#include <core/request.h>
#include <core/target.h>
#include <tools/error_report.h>
#include <iostream>

Controller::Controller(const std::string & name, uint32_t pool) :
    ActiveObject(name, pool), Logged("ctrl"){
    }

Controller::~Controller() {
    workflows.clear();
}

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

bool Controller::addWorkflow(WorkflowPtr workflow) {
    BOOST_LOG_SEV(logger, Info) << getName() << " Add workflow : " << workflow << std::endl;
    // check first if workflow is "addable"
    
    auto er = workflow->getStateMachine()->validate();
    if(er.isSet()) {
        BOOST_LOG_SEV(logger, Error) << getName() << " Can't add provided workflow " << workflow->getName() << " due to : " << er.getErrorMessage();
        return false;
    }
    
    workflow->setController(shared_from_this());
    workflows[workflow->getName()] = workflow;
    return true;
}


OSTREAM_HELPER_IMPL(Controller, obj) {
    out << "[Controller] name: " << obj.getName() << ", pool: " << obj.getPoolSize();
    return out;
}