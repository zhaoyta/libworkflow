#include <service/controller_manager.h>
#include <core/controller.h>
#include <core/request.h>
#include <core/target.h>
#include <service/client_manager.h>
#include <boost/uuid/uuid_generators.hpp>

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
    boost::interprocess::scoped_lock<boost::recursive_mutex> sl(*mutex);

    if(controllers.count(ctrl) > 0)
        return controllers[ctrl];
    //! @todo add log failed to find controller ... 
    return ControllerPtr();
}

void ControllerManager::perform(RequestPtr req) {
    getIOService()->dispatch(boost::bind<void>([&](RequestPtr req) {
        boost::interprocess::scoped_lock<boost::recursive_mutex> sl(*mutex);

        if(req->getTarget().target == ETargetAction::NoReply) {
            //! @todo log nothing to do :)
            BOOST_LOG_SEV(logger, Debug) << this << req->logRequest() << " Well it's a NoReply thus aborting it.";
            return;
        }
        
        if(req->getTarget().client_id != boost::uuids::nil_uuid()) {
            ClientManager::getInstance()->perform(req);
        }
        
        bool able = false;
        if(controllers.count(req->getTarget().controller) > 0 )
            able = controllers[req->getTarget().controller]->perform(req);
        else {
            able = controllers["default"]->perform(req);
        }
        
        if(not able) {
            BOOST_LOG_SEV(logger, Warn) << this << req->logRequest() << " Can't process this request, no apt controller found.";
        }
    } , req));
}

void ControllerManager::started() {
    auto controller = ControllerPtr(new Controller("default", default_pool));
    
    registerController(controller);
}

void ControllerManager::registerController(ControllerPtr controller) {
    boost::interprocess::scoped_lock<boost::recursive_mutex> sl(*mutex);

    BOOST_LOG_SEV(logger, Info) << "Setting new controller " << controller;
    controllers[controller->getName()] = controller;
}

OSTREAM_HELPER_IMPL(ControllerManager, obj) {
    out << "[ControllerManager]";
    return out;
}