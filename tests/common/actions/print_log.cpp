#include <tests/common/actions/print_log.h>
#include <tools/property_set.h>

PrintLog::PrintLog() : Action("PrintLog") {
    properties()->defineStringProperty("log", "Default Message", "Message to display", true);
}

Result PrintLog::perform(SessionPtr session) const{
    BOOST_LOG_SEV(logger, Info) << fingerprint(session) << " " << stringValue(session, "log") ;
    return done();
}
