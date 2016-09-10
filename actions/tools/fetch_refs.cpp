#include <actions/tools/fetch_refs.h>
#include <contexts/refs_storage_ctx.h>
#include <tools/action_factory.h>
#include <tools/context_factory.h>
#include <service/controller_manager.h>
#include <core/controller.h>
#include <core/workflow.h>

FetchRefs::FetchRefs(): Action("FetchRefs") {
    defineOutput("refs", CHECKER(RefsStorageCtx));
    
    properties()->defineBoolProperty("fetch_actions", true, "Fetch actions");
    properties()->defineBoolProperty("fetch_contexts", true, "Fetch contexts");
    properties()->defineBoolProperty("fetch_workflows", true, "Fetch workflows");
    
}

FetchRefs::~FetchRefs() {}

Result FetchRefs::perform(SessionPtr session) const {
    auto res = RefsStorageCtxPtr(new RefsStorageCtx());
    std::map<std::string, std::string> actions;
    std::map<std::string, std::string> contexts;
    std::map<std::string, std::map<std::string, std::string> > workflows;
    if(boolProperty(session, "fetch_actions")) {
        for(const auto & kv: ActionFactory::getBuilders()) {
            std::string str;
            kv.second->create()->str_save(str);
            actions[kv.first] = str;
        }
        res->setActions(actions);
    }
    if(boolProperty(session, "fetch_contexts")) {
        for(const auto & kv: ContextFactory::getBuilders()) {
            std::string str;
            kv.second->create()->str_save(str);
            contexts[kv.first] = str;
        }
        res->setContexts(contexts);
    }
    if(boolProperty(session, "fetch_workflows")) {
        for(const auto & kv: ControllerManager::getInstance()->getControllers()) {
            for(const auto & kw: kv.second->getWorkflows()) {
                std::string str;
                kw.second->str_save(str);
                workflows[kv.first][kw.first] = str;
            }
        }
        res->setWorkflows(workflows);
    }
    
    setOutput(session, "refs", res);
    return done();
}