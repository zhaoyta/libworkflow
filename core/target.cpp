#include <core/target.h>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/string_generator.hpp>


BEGIN_ENUM_IMPL(TargetAction) {
    {"DefaultAction",   (uint32_t)ETargetAction::DefaultAction},
    {"Status",          (uint32_t)ETargetAction::Status},
    {"Interrupt",       (uint32_t)ETargetAction::Interrupt},
    {"Error",           (uint32_t)ETargetAction::Error},
    {"Reply",           (uint32_t)ETargetAction::Reply},
    {"NoReply",         (uint32_t)ETargetAction::NoReply},
};
END_ENUM_IMPL(TargetAction);


Target::Target() : Jsonable(),
    controller("default"),
    identifier(boost::uuids::random_generator()()),
    client_id(boost::uuids::nil_generator()()),
    target(ETargetAction::DefaultAction),
    action(-1){}

Target::Target( const std::string & workflow) : Jsonable(),
controller("default"),
workflow(workflow),
identifier(boost::uuids::random_generator()()),
client_id(boost::uuids::nil_generator()()),
target(ETargetAction::DefaultAction),
action(-1){}

Target::Target(const std::string & controller, const std::string & workflow) : Jsonable(),
    controller(controller),
    workflow(workflow),
identifier(boost::uuids::random_generator()()),
client_id(boost::uuids::nil_generator()()),
    target(ETargetAction::DefaultAction),
    action(-1){}


Target::~Target() {}

bool Target::operator<(const Target & t) const{
    return identifier < t.identifier;
}

std::string Target::shortId() const {
    return ::shortId(identifier);
}

void Target::save(boost::property_tree::ptree & root) const {
    root.put("controller", controller);
    root.put("workflow", workflow);
    root.put("identifier", to_string(identifier));
    root.put("client_id", to_string(client_id));
    root.put("execution_level", execution_level);
    root.put("action_id", action);
    std::stringstream str;
    str << target;
    root.put("target", str.str());
    
}

void Target::load(const boost::property_tree::ptree & root) {
    GET_OPT(root, controller, std::string, "controller");
    GET_OPT(root, workflow, std::string, "workflow");
    
    boost::uuids::string_generator gen;
    std::string rid;
    GET_OPT(root, rid, std::string, "identifier");
    identifier = gen(rid);
    std::string cid;
    GET_OPT(root, cid, std::string, "client_id");
    client_id = gen(cid);
    
    GET_OPT(root, execution_level, uint32_t, "execution_level");
    GET_OPT(root, action, int32_t, "action_id");
    
    std::string target_str;
    GET_OPT(root, target_str, std::string, "target");
    target = CTargetAction::valueFromString(target_str);
    
}

OSTREAM_HELPER_IMPL(Target, obj) {
    out << "[Target] { "
        << "  controller:" << obj.controller
        << ", workflow:" << obj.workflow
        << ", id: " << obj.shortId()
        << ", target: " << obj.target
        << ", client: " << shortId(obj.client_id)
        << " }";
    return out;
}