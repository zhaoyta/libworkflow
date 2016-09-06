#include <core/bindings.h>

InputBinding::InputBinding(const std::string & workflow_input, int32_t action_id, const std::string & action_input) :
    workflow_input(workflow_input),
    action_id(action_id),
action_input(action_input) {
    
}

InputBinding::InputBinding() {}
InputBinding::~InputBinding() {
    
}

int32_t InputBinding::getActionId() const {
    return action_id;
}

const std::string & InputBinding::getActionInput() const {
    return action_input;
}

const std::string & InputBinding::getWorkflowInput() const {
    return workflow_input;
}

void InputBinding::save(boost::property_tree::ptree & root) const {
    
}

void InputBinding::load(const boost::property_tree::ptree & root) {
    
}


OutputBinding::OutputBinding() {
    
}



OutputBinding::OutputBinding(const std::string & from_action_output,
                             Step to_action_id, const std::string & to_action_input) :
    from_action_output(from_action_output),
to_action_id((int32_t)to_action_id), to_action_input(to_action_input) {
    
}

OutputBinding::OutputBinding(const std::string & from_action_output,
                             int32_t to_action_id, const std::string & to_action_input) :
    from_action_output(from_action_output),
to_action_id(to_action_id), to_action_input(to_action_input) {
    
}

OutputBinding::OutputBinding(Step from_action_id, const std::string & from_action_output,
                             Step to_action_id, const std::string & to_action_input) :
from_action_id((int32_t)from_action_id), from_action_output(from_action_output),
to_action_id((int32_t)to_action_id), to_action_input(to_action_input) {

}

OutputBinding::OutputBinding(int32_t from_action_id, const std::string & from_action_output,
                             int32_t to_action_id, const std::string & to_action_input) :
    from_action_id(from_action_id), from_action_output(from_action_output),
    to_action_id(to_action_id), to_action_input(to_action_input) {
    
}


OutputBinding::OutputBinding(int32_t from_action_id, const std::string & from_action_output,
                             Step to_action_id, const std::string & to_action_input) :
from_action_id(from_action_id), from_action_output(from_action_output),
to_action_id((int32_t)to_action_id), to_action_input(to_action_input) {
    
}

OutputBinding::~OutputBinding() {
    
}


void OutputBinding::setFromActionId(int32_t aid) {
    from_action_id = aid;
}

void OutputBinding::setFromActionId(Step s) {
    from_action_id = (int32_t)s;
}

int32_t OutputBinding::getFromActionId() const {
    return from_action_id;
}

const std::string & OutputBinding::getFromActionOutput() const {
    return from_action_output;
}

int32_t OutputBinding::getToActionId() const {
    return to_action_id;
}

const std::string & OutputBinding::getToActionInput() const {
    return to_action_input;
}

void OutputBinding::save(boost::property_tree::ptree & root) const {
    
}

void OutputBinding::load(const boost::property_tree::ptree & root) {
    
}
