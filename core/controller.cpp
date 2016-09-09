#include <core/controller.h>
#include <core/workflow.h>
#include <core/controller_spawn.h>
#include <core/state_machine.h>
#include <core/request.h>
#include <core/target.h>
#include <tools/error_report.h>
#include <service/controller_manager.h>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid.hpp>
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

void Controller::dropWorkflow(const std::string & key) {
    workflows.erase(key);
}

WorkflowPtr Controller::getWorkflow(const std::string & name) const {
    if(workflows.count(name) > 0 ) {
        return workflows.at(name);
    }
    return WorkflowPtr();
}

void Controller::requestFinished(RequestPtr) {
    
}


TemporaryController::TemporaryController(uint32_t pool) : Controller("temporary", pool) {}
TemporaryController::~TemporaryController() {
}

bool TemporaryController::perform(RequestPtr req) {
    if(req->getWorkflowJson().empty()) {
        BOOST_LOG_SEV(logger, Error) << req->logRequest() << getName() << " controller can't execute this request, as it doesn't provide a workflow.";
        
        auto reply = Request::createReply(req);
        ErrorReportPtr er = ErrorReportPtr(new ErrorReport());
        er->setError("temporary.empty.workflow", "Empty workflow provided, can't execute request.");
        reply->setErrorReport(er);
        ControllerManager::getInstance()->perform(reply);
        return false;
    }
    WorkflowPtr wkf(new Workflow(""));
    wkf->str_load(req->getWorkflowJson());
    auto er = wkf->getStateMachine()->validate();
    if(er.isSet()) {
        BOOST_LOG_SEV(logger, Error) << req->logRequest() << getName() << " provided workflow isn't valid: " << er.getErrorMessage();
        auto reply = Request::createReply(req);
        ErrorReportPtr erp = ErrorReportPtr(new ErrorReport(er));
        reply->setErrorReport(erp);
        ControllerManager::getInstance()->perform(reply);
    }
    
    auto wkf_prefix = shortId(boost::uuids::random_generator()(), 3);
    wkf->setName(wkf_prefix + wkf->getName());
    req->getTarget().workflow = wkf_prefix + wkf->getName();
    addWorkflow(wkf);
    BOOST_LOG_SEV(logger, Trace) << req->logRequest() << getName() << " adding temporary workflow: " << req->getReply().workflow;
    
    return true;
}

void TemporaryController::requestFinished(RequestPtr req) {
    dropWorkflow(req->getReply().workflow);
    BOOST_LOG_SEV(logger, Trace) << req->logRequest() << getName() << " dropping temporary workflow: " << req->getReply().workflow;
}



OSTREAM_HELPER_IMPL(Controller, obj) {
    out << "[Controller] name: " << obj.getName() << ", pool: " << obj.getPoolSize();
    return out;
}