#include <core/workflow.h>
#include <core/session.h>
#include <tools/timed.h>


Workflow::Workflow(ControllerPtr ctr) :
    stateMachine(new StateMachine()),
    controller(ctr){
    
}

Workflow::~Workflow() {
    
}


Workflow::RunningSession::RunningSession() :
    original(request),
    session(new Session()),
    timed(new Timed()){
    
}

Workflow::RunningSession::~RunningSession() {
    
}

bool Workflow::perform(RequestPtr ) {
    
}

StateMachinePtr Workflow::getStateMachine() {
    
}


bool Workflow::canExecuteRequest(RequestPtr) {
    
}

void Workflow::requestTimedOut(boost::uuids::uuid id) {
    
}

void Workflow::setTimeout(double d) {
    timeout = d;
}