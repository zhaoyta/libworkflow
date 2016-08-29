#include <core/context.h>


Context::Context(): Jsonable(), type("Context") {}
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

SkipCtx::SkipCtx() : Context("SkipCtx") {}
SkipCtx::~SkipCtx(){}

GroupedCtx::GroupedCtx() : Context("GroupedCtx") {}
GroupedCtx::~GroupedCtx() {}

size_t GroupedCtx::size() {
    return contexts.size();
}

ContextPtr GroupedCtx::getContext(const std::string & ctx){
    if(contexts.count(ctx) > 0)
        return contexts[ctx];
    return ContextPtr();
}

void GroupedCtx::setContext(const std::string & ctx_name, ContextPtr ctx) {
    if(ctx)
        contexts[ctx_name] = ctx;
}

std::map<std::string, ContextPtr> & GroupedCtx::getContexts() {
    return contexts;
}

