#include <tests/common/actions/do_next.h>

DoNext::DoNext() : Action("DoNext") {}

Result DoNext::perform(SessionPtr session) const{
    BOOST_LOG_SEV(logger, Info) << fingerprint(session) << " Do next performed.";
    return done();
}
