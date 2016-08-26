#include <core/context.h>


Context::Context(const std::string & type) : Jsonable(),
type(type) {
    
}

Context::~Context() {
    
}

const std::string & Context::getType() const {
    return type;
}


void Context::save(boost::property_tree::ptree & root) const {
    
}

void Context::load(const boost::property_tree::ptree & root) {
    
}


ListCtx::ListCtx(const std::string & type) : Context(type) {
    
}

ListCtx::~ListCtx() {
    
}

bool ListCtx::isEmpty() const {
    return contexts.empty();
}

void ListCtx::add(ContextPtr ctx) {
    if(ctx)
        contexts.push_back(ctx);
}

void ListCtx::clear() {
    contexts.clear();
}

void ListCtx::save(boost::property_tree::ptree & root) const {
    
}

void ListCtx::load(const boost::property_tree::ptree & root) {
    
}
