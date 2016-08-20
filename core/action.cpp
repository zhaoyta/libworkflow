#include <core/action.h>
#include <core/action_priv.h>
#include <core/action_meta_priv.h>
#include <core/context.h>
#include <core/property_set.h>
#include <core/request.h>
#include <core/session.h>
#include <core/state_machine.h>


Action::Action(const std::string & name): action_id(0), name(name), p(new ActionPriv(this)), pmeta( new ActionMetaPriv(this)), properties(new PropertySet()) {
    propertyset->setGuarded(true);
    
}

Action::~Action() {
    delete p;
    delete pmeta;
}

bool Action::checkInputs(SessionPtr, ErrorReport &) const {
    
}

bool Action::checkOutputs(SessionPtr, ErrorReport &) const {
    
}

Result Action::perform(SessionPtr) const {
    
}

Result Action::replyReceived(SessionPtr, RequestPtr) const {
    
}

bool Action::canPerform(SessionPtr, ErrorReport &) const {
    
}

const std::string & Action::getName() const{
    
}

void Action::setName(const std::string & name) {
    
}

int32_t Action::getActionId() const {
    
}

PropertySetPtr Action::properties() {
    
}

StateMachinePtr Action::getStateMachine() const {
    
}

Result Action::done() const {
    
}

Result Action::wait() const {
    
}

Result Action::async() const {
    
}

Result Action::error(const ErrorReport &) const {
    
}

void Action::asyncDone() const {
    
}

void Action::asyncWait() const {
    
}

void Action::asyncError(const ErrorReport &) const {
    
}


std::string Action::fingerprint(SessionPtr) const {
    
}

double Action::doubleValue(SessionPtr, const std::string & key, double default) const {
    
}

bool Action::boolValue(SessionPtr, const std::string & key, bool default) const {
    
}

std::string Action::stringValue(SessionPtr, const std::string & key, const std::string & default ) const {
    
}

uint32_t Action::uintValue(SessionPtr, const std::string & key, uint32_t default) const {
    
}

ContextPtr Action::customValue(SessionPtr, const std::string & key, ContextPtr default) const {

}

void Action::defineInput(const std::string & name, TypeChecker * , bool mandatory) {
    
}

void Action::defineInput(const PutDefinition & ) {
    
}

void Action::defineOutput(const std::string & name, TypeChecker * , bool mandatory = true) {
    
}

void Action::defineOutput(const PutDefinition &) {
    
}

ContextPtr Action::getInput(SessionPtr, const std::string & name) {
    
}

void Action::setOutput(SessionPtr, const std::string & name, ContextPtr) {
    
}

