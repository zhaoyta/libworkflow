#include <core/request.h>
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <tools/property_set.h>
#include <iomanip>

Request::Request(): Jsonable() {
    reply.target = ETargetAction::NoReply;
    target.target = ETargetAction::NoReply;
}

Request::Request(const Target & target): Jsonable(), target(target), request_id(target.identifier) , bypass(new PropertySet()){
    reply.target = ETargetAction::NoReply;
}

Request::Request(const Target & target, const Target & reply): Jsonable(), target(target), reply(reply), request_id(target.identifier)  , bypass(new PropertySet()){
    
}

RequestPtr Request::createReply(RequestPtr request) {
    if(request) {
        auto rep = RequestPtr(new Request(request->getReply(), request->getTarget()));
        rep->setRequestId(request->getRequestId());
        rep->setClientId(request->getClientId());
        // thats the basics.
        
        // now the fun part :)
        
        if(rep->getClientId() != boost::uuids::nil_uuid()) {
            // ha ah ! it was sent from another client. we must reroute appropriately.
            
            //! @todo add a new Request to Workflow ToClient, prepare a nice SendCtx with this request as child.
        }
        return rep;
    }
    return RequestPtr();
}

Request::~Request() {
    
}


boost::uuids::uuid Request::getClientId() const {
    return client_id;
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

void Request::setClientId(const boost::uuids::uuid & id) {
    client_id = id;
}

PropertySetPtr Request::getBypass() {
    return bypass;
}

std::map<int32_t, PropertySetPtr> & Request::getActionBypasses() {
    return action_bypasses;
}

ContextPtr Request::getContext() {
    return context;
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

ErrorReportPtr Request::getErrorReport() {
    return report;
}

void Request::save(boost::property_tree::ptree & root) const {
    
}

void Request::load(const boost::property_tree::ptree & root) {
    
}

ControllerSpawnPtr Request::getControllerSpawn() {
    return spawn;
}

void Request::setControllerSpawn(ControllerSpawnPtr sp) {
    spawn = sp;
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
