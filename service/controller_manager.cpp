#include <service/controller_manager.h>
#include <core/controller.h>
#include <core/request.h>

static ControllerManagerPtr instance;

ControllerManager::ControllerManager(uint32_t default_pool) :
    ActiveObject("ControllerManager",3, true),
    default_pool(default_pool)
{
    setNamespace("ctrl.mngr");
    if(default_pool == 0)
        default_pool = 1;
    
}
ControllerManager::~ControllerManager() {}

ControllerManagerPtr ControllerManager::getInstance() {
    if(not instance)
        instance.reset(new ControllerManager());
    return instance;
}


ControllerPtr ControllerManager::getController(const std::string & ctrl) {
    if(controllers.count(ctrl) > 0)
        return controllers[ctrl];
    //! @todo add log failed to find controller ... 
    return ControllerPtr();
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
    BOOST_LOG_SEV(logger, Info) << "Setting new controller " << controller;
    controllers[controller->getName()] = controller;
}

OSTREAM_HELPER_IMPL(ControllerManager, obj) {
    out << "[ControllerManager]";
    return out;
}