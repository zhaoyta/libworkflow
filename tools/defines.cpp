#include <tools/defines.h>

std::string shortId(const boost::uuids::uuid & uid, int length ){
    return to_string(uid).substr(1,length);
}
