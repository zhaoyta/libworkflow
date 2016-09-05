#include <core/session.h>
#include <core/request.h>
#include <tools/property_set.h>
#include <core/context.h>
#include <core/controller_spawn.h>
#include <core/target.h>

BEGIN_ENUM_IMPL(ExecutionStatus) {
    {"Unplanned",   (uint32_t)EExecutionStatus::Unplanned},
    {"Pending",     (uint32_t)EExecutionStatus::Pending},
    {"Planned",     (uint32_t)EExecutionStatus::Planned},
    {"Skipped",     (uint32_t)EExecutionStatus::Skipped},
    {"Waiting",     (uint32_t)EExecutionStatus::Waiting},
    {"Async",       (uint32_t)EExecutionStatus::Async},
    {"Done",        (uint32_t)EExecutionStatus::Done},
};
END_ENUM_IMPL(ExecutionStatus);

Session::Session(): current_execution_level(1), finished(false) {
    
}

Session::~Session() {
    inputs.clear();
    outputs.clear();
    bypass.reset();
    action_bypasses.clear();
}

PropertySetPtr Session::getBypass() {
    return bypass;
}

PropertySetPtr Session::getBypass(int32_t action_id) {
    if(action_bypasses.count(action_id) == 0)
        action_bypasses[action_id].reset(new PropertySet());
    return action_bypasses[action_id];
}

std::set<int32_t> & Session::getNexts() {
    return nexts;
}

std::set<int32_t> & Session::getPendings() {
    return pendings;
}

std::set<Target> & Session::getSubQueries() {
    return subqueries;
}

std::map<int32_t, EExecutionStatus> & Session::getStatus() {
    return status;
}

EExecutionStatus Session::getStatus(int32_t action_id) const {
    return status.at(action_id);
}

void Session::setStatus(int32_t action_id, EExecutionStatus s) {
    status[action_id] = s;
}

std::map<int32_t, std::map<std::string, ContextPtr> > & Session::getInputs() {
    return inputs;
}

std::map<int32_t, std::map<std::string, ContextPtr> > & Session::getOutputs(){
    return outputs;
}


void Session::setOutput(int32_t action_id, const std::string & output, ContextPtr ctx) {
    outputs[action_id][output] = ctx;
}

ContextPtr Session::getOutput( int32_t aid, const std::string & output) {
    if(outputs.count(aid) > 0 and outputs[aid].count(output) > 0 )
        return outputs[aid][output];
    return ContextPtr();
}

void Session::setInput(int32_t action_id, const std::string & input, ContextPtr ctx) {
    inputs[action_id][input] = ctx;
}

ContextPtr Session::getInput( int32_t action_id, const std::string & input) {
    if(inputs[action_id].count(input) > 0) {
        return inputs[action_id][input];
    }
    return ContextPtr();
}

void Session::addSubQuery(const Target & t) {
    subqueries.insert(t);
}

void Session::removeSubQuery(const Target & t) {
    subqueries.erase(t);
}

uint32_t Session::getCurrentExecutionLevel() const {
    return current_execution_level;
}

void Session::upCurrentExecutionLevel() {
    current_execution_level++;
}

RequestPtr Session::getOriginalRequest() const {
    return request;
}

const std::vector<RequestPtr> & Session::getRequests() const {
    return requests;
}

RequestPtr Session::getLastRequest() const {
    if(requests.size() == 0)
        return RequestPtr();
    auto it = requests.rbegin();
    return * it;
}

ControllerSpawnPtr Session::getControllerSpawn() {
    if(request)
        return request->getControllerSpawn();
    return ControllerSpawnPtr();
}

void Session::pushRequest(RequestPtr req) {
    if(not request) {
        // initial request found !
        request = req;
        //! recovering bypasses ...
        bypass = req->getBypass();
        action_bypasses = req->getActionBypasses();
    }
    requests.push_back(req);
}

bool Session::hasFinished() {
    return finished;
}

void Session::setFinished() {
    finished = true;
}

OSTREAM_HELPER_IMPL(Session, obj) {
    out << "[Session]";
    return out;
}

