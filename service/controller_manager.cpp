#include <service/controller_manager.h>
#include <core/controller.h>
#include <core/request.h>

ControllerManager * ControllerManager::instance = NULL;

ControllerManager::ControllerManager(uint32_t default_pool) :
    ActiveObject("ControllerManager"),
    default_pool(default_pool)
{
    if(default_pool == 0)
        default_pool = 1;
}
ControllerManager::~ControllerManager() {}

ControllerManager * ControllerManager::getInstance() {
    if(not instance)
        instance = new ControllerManager();
    return instance;
}

void ControllerManager::perform(RequestPtr req) {
    getIOService()->dispatch(boost::bind<void>([&](RequestPtr req) {
        if(req->getTarget().target == ETargetAction::NoReply) {
            //! @todo log nothing to do :)
            return;
        }
        
        bool able = false;
        if(controllers.count(req->getTarget().controller) > 0 )
            able = controllers[req->getTarget().controller]->perform(req);
        else {
            able = controllers["default"]->perform(req);
        }
        
        if(not able) {
            //! @todo add log stating impossibility to find an apt controller.
        }
    } , req));
}

void ControllerManager::started() {
    auto controller = ControllerPtr(new Controller("default", default_pool));
    registerController(controller);
}

void ControllerManager::registerController(ControllerPtr controller) {
    controllers[controller->getName()] = controller;
}