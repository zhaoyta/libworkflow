#include <tools/error_report.h>

ErrorReport::ErrorReport(): Jsonable(), boost::enable_shared_from_this<ErrorReport>(){}
ErrorReport::ErrorReport(const Target & target, const std::string & error_key, const std::string & error_msg): Jsonable(), boost::enable_shared_from_this<ErrorReport>(), target(target), error_message(error_msg), error_key(error_key) {}
ErrorReport::ErrorReport(const Target & target, ErrorReportPtr parent, const std::string & error_key , const std::string & error_msg ): Jsonable(), boost::enable_shared_from_this<ErrorReport>(), target(target), error_message(error_msg), error_key(error_key), parent(parent) {}
ErrorReport::~Errorreport() {}

const std::string & ErrorReport::getErrorMessage() const {
    if(not parent)
        return error_message;
    else
        return getAncestry().begin()->getErrorMessage();
}

const std::string & ErrorReport::getErrorKey() const {
    if(not parent)
        return error_key;
    else
        return getAncestry().begin()->getErrorKey();
}

const ErrorReportPtr ErrorReport::getParent() const {
    return parent;
}

const std::vector<ErrorReportPtr> ErrorReport::getAncestry() const {
    std::vector<ErrorReportPtr> ancestry;
    if(not parent) {
        ancestry.push_back(shared_from_this());
        return ancestry;
    } else {
        ancestry = parent->getAncestry();
        ancestry.push_back(shared_from_this());
        return ancestry;
    }
}


void ErrorReport::save(boost::property_tree::ptree & root) const {
    
}

void ErrorReport::load(const boost::property_tree::ptree & root) {
    
}