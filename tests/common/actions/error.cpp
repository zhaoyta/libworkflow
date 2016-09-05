#include <tests/common/actions/error.h>

ErrorAction::ErrorAction() : Action("ErrorAction") {
    properties()->defineStringProperty("error_message", "Default error message", "Error message.");
}

Result ErrorAction::perform(SessionPtr session) const{
    return error(session, "error.action.break", stringProperty(session,"error_message"));
}
