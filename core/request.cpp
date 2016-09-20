#include <core/request.h>
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <tools/property_set.h>
#include <core/context.h>
#include <iomanip>
#include <boost/uuid/string_generator.hpp>

Request::Request(): Jsonable(), context( new GroupedCtx()), bypass(new PropertySet()){
    reply.target = ETargetAction::NoReply;
    target.target = ETargetAction::NoReply;
}

Request::Request(const Target & target): Jsonable(), target(target)
    , context( new GroupedCtx()), request_id(target.identifier)
    , bypass(new PropertySet()){
    reply.target = ETargetAction::NoReply;
}

Request::Request(const Target & target, const Target & reply): Jsonable()
    , target(target), reply(reply)
    , request_id(target.identifier), context( new GroupedCtx())
    , bypass(new PropertySet()){
    
}

RequestPtr Request::createReply(RequestPtr request) {
    if(request) {
        auto rep = RequestPtr(new Request(request->getReply(), request->getTarget()));
        rep->setRequestId(request->getRequestId());
        return rep;
    }
    return RequestPtr();
}

Request::~Request() {
    
}


boost::uuids::uuid Request::getRequestId() const {
    return request_id;
}

boost::uuids::uuid Request::getId() const {
    return target.identifier;
}

std::string Request::shortRequestId() const {
    return shortId(getRequestId());
}

void Request::setId(const boost::uuids::uuid & id) {
    target.identifier = id;
}

void Request::setRequestId(const boost::uuids::uuid & id) {
    request_id = id;
}

PropertySetPtr Request::getBypass() const {
    return bypass;
}

void Request::setBypass(PropertySetPtr set) {
    bypass = set;
}


PropertySetPtr Request::getActionBypasses(int action_id) {
    if(action_bypasses.count(action_id) == 0)
        action_bypasses[action_id].reset(new PropertySet());
    return action_bypasses[action_id];
}

std::map<int32_t, PropertySetPtr> & Request::getActionBypasses() {
    return action_bypasses;
}

GroupedCtxPtr Request::getContext() const {
    return context;
}

ContextPtr Request::getContext(const std::string & key) const {
    return context->getContext(key);
}

void Request::setContext(const std::string & key, Context* ptr) {
    setContext(key, ContextPtr(ptr));
}

void Request::setContext(const std::string & key, ContextPtr ctx) {
    context->setContext(key,ctx);
}

const Target & Request::getTarget() const {
    return target;
}

const Target & Request::getReply() const {
    return reply;
}


Target & Request::getTarget() {
    return target;
}

Target & Request::getReply() {
    return reply;
}

void Request::setErrorReport(ErrorReportPtr er) {
    report = er;
}

ErrorReportPtr Request::getErrorReport() const {
    return report;
}

void Request::save(boost::property_tree::ptree & root) const {
    PUT_CHILD(root, target, "target");
    PUT_CHILD(root, reply, "reply");
    root.put("request_id", to_string(request_id));
    PUT_CHILD(root, (*context), "context");
    PUT_CHILD(root, (*bypass), "bypass");
    
    boost::property_tree::ptree cmap;
    for(const auto & kv: action_bypasses) {
        boost::property_tree::ptree citem;
        kv.second->save(citem);
        std::stringstream str;
        str << kv.first;
        cmap.add_child(str.str(), citem);
    }
    
    root.add_child("action_bypasses", cmap);
    
    if(report)
        PUT_CHILD(root, (*report), "error_report");
    
    // ignore controller spawn, that's set by controller upon execution.
    root.put("workflow_json", workflow_json);
}

void Request::load(const boost::property_tree::ptree & root) {
    GET_CHILD(root, target, "target");
    GET_CHILD(root, reply, "reply");
    
    boost::uuids::string_generator gen;
    std::string rid;
    GET_OPT(root, rid, std::string, "request_id");
    
    request_id = gen(rid);
    
    GET_CHILD(root, (*context), "context");
    GET_CHILD(root, (*bypass), "bypass");
    
    auto cmap = root.get_child_optional("action_bypasses");
    if(cmap) {
        for(const auto & kv: *cmap) {
            std::stringstream str(kv.first.data());
            int32_t action_id;
            str >> action_id;
            auto pset = PropertySetPtr(new PropertySet());
            pset->load(kv.second);
            action_bypasses[action_id] = pset;
        }
    }
    
    auto creport = root.get_child_optional("report");
    if(creport) {
        // isn't set by default.
        report.reset(new ErrorReport);
        GET_CHILD(root, (*report), "report");
    }
    
    GET_OPT(root, workflow_json, std::string, "workflow_json");
}

ControllerSpawnPtr Request::getControllerSpawn() const {
    return spawn;
}

void Request::setControllerSpawn(ControllerSpawnPtr sp) {
    spawn = sp;
}

const std::string & Request::getWorkflowJson() const {
    return workflow_json;
}

void Request::setWorkflowJson(const std::string & json) {
    workflow_json = json;
}

void Request::setTarget(const Target & t) {
    target = t;
}

void Request::setReply(const Target & t) {
    reply = t;
}

std::string Request::logRequest() const {
    std::stringstream str;
    str << "[R " << shortId(request_id,4) << "-" <<shortId(target.identifier, 4) << "]";
    return str.str();
}

OSTREAM_HELPER_IMPL(Request, obj) {
    out << "[Request] { "
        << " id: " << obj.shortRequestId()
        << ", target: " << obj.getTarget()
        << ", reply: " << obj.getReply()
        << "}";
    return out;
}
