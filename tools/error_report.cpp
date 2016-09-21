#include <tools/error_report.h>
#include <core/request.h>

ErrorReport::ErrorReport(): Jsonable(), boost::enable_shared_from_this<ErrorReport>(){}

ErrorReport::ErrorReport(const ErrorReport & ref) :
    Jsonable(),
target(ref.target),
error_message(ref.error_message),
error_key(ref.error_key),
parent(ref.parent) {
    
}

ErrorReport::ErrorReport(const Target & target, const std::string & error_key, const std::string & error_msg):
    Jsonable(),
    boost::enable_shared_from_this<ErrorReport>(),
    target(target),
    error_message(error_msg),
    error_key(error_key) {}

ErrorReport::ErrorReport(const Target & target, ErrorReportPtr parent, const std::string & error_key , const std::string & error_msg ):
    Jsonable(),
    boost::enable_shared_from_this<ErrorReport>(),
    target(target),
    error_message(error_msg),
    error_key(error_key),
    parent(parent) {}

ErrorReport::~ErrorReport() {}

void ErrorReport::setError(const std::string & errk, const std::string & errm) {
    error_message = errm;
    error_key = errk;
}

bool ErrorReport::isSet() const {
    if(not parent and error_key.empty())
        return false;
    return true;
}

const std::string & ErrorReport::getErrorMessage()  {
    if(not parent)
        return error_message;
    else
        return (*getAncestry().begin())->getErrorMessage();
}

const std::string & ErrorReport::getErrorKey()  {
    if(not parent)
        return error_key;
    else
        return (*getAncestry().begin())->getErrorKey();
}

const ErrorReportPtr ErrorReport::getParent()  {
    return parent;
}

const std::vector<ErrorReportPtr> ErrorReport::getAncestry() {
    std::vector<int32_t> tt;
    tt.push_back(1);
    
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

void ErrorReport::setRequest(RequestPtr req) {
    original = req;
}

RequestPtr ErrorReport::getRequest() const {
    return original;
}

void ErrorReport::save(boost::property_tree::ptree & root) const {
    PUT_CHILD(root, target, "target");
    root.put("message", error_message);
    root.put("key", error_key);
    PUT_CHILD(root, *parent, "parent");
    if(original)
        PUT_CHILD(root, *original, "original");
}

void ErrorReport::load(const boost::property_tree::ptree & root) {
    GET_OPT(root, error_message, std::string, "message");
    GET_OPT(root, error_key, std::string, "key");
    
    auto otarget = root.get_child_optional("target");
    if(otarget)
        target.load(*otarget);
    
    auto oparent = root.get_child_optional("parent");
    if(oparent) {
        parent.reset(new ErrorReport());
        parent->load(*oparent);
    }
    
    auto ooriginal = root.get_child_optional("original");
    if(ooriginal){
        original.reset(new Request());
        original->load(*ooriginal);
    }
}