#include <core/target.h>
#include <boost/uuid/uuid_generators.hpp>

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
    
}

void Target::load(const boost::property_tree::ptree & root) {
    
}

OSTREAM_HELPER_IMPL(Target, obj) {
    out << "[Target] { "
        << "  controller:" << obj.controller
        << ", workflow:" << obj.workflow
        << ", id: " << obj.shortId()
        << ", target: " << obj.target
        << " }";
    return out;
}