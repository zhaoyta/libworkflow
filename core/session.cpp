
Session::Session(), current_execution_level(1) {
    
}

Session::~Session() {
    
}

PropertySetPtr Session::getBypass() {
    return bypass;
}

PropertySetPtr Session::getBypass(int32_t action_id) {
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

std::map<int32_t, std::map<std::string, ContextPtr> > & Session::getInputs() {
    return inputs;
}

std::map<int32_t, std::map<std::string, ContextPtr> > & Session::getOutputs(){
    return outputs;
}


void Session::setOutput(int32_t action_id, const std::string & output, ContextPtr ctx) {
    outputs[action_id][output] = ctx;
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
