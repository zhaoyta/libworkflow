#include <core/context.h>
#include <tools/context_factory.h>
#include <tools/logged.h>

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
    root.put("type", type);
}

void Context::load(const boost::property_tree::ptree & root) {
    //! type shoudln't be loaded ...
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
    Context::save(root);
    boost::property_tree::ptree plist;
    for(const auto & ctx: contexts) {
        boost::property_tree::ptree citem;
        ctx->save(citem);
        plist.push_back(std::make_pair("", citem));
    }
    root.add_child("items", plist);
}

void ListCtx::load(const boost::property_tree::ptree & root) {
    auto olist = root.get_child_optional("items");
    if(olist) {
        for(const auto & kv: *olist) {
            const auto & citem = kv.second;
            std::string type = citem.get<std::string>("type");
            ContextPtr ctx = ContextFactory::create(type);
            if(ctx) {
                ctx->load(citem);
                contexts.push_back(ctx);
            } else {
                GLOB_LOGGER("json.ctx");
                BOOST_LOG_SEV(logger, Error) << " Failed to find an appropriate factory for context type: " << type;
            }
        }
    }
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


void GroupedCtx::save(boost::property_tree::ptree & root) const {
    Context::save(root);
    
    boost::property_tree::ptree pitems;
    for(const auto & kv: contexts) {
        boost::property_tree::ptree pitem;
        kv.second->save(pitem);
        pitems.add_child(kv.first, pitem);
    }
    root.add_child("items", pitems);
}

void GroupedCtx::load(const boost::property_tree::ptree & root) {
    auto olist = root.get_child_optional("items");
    if(olist) {
        for(const auto & kv: *olist) {
            const auto & citem = kv.second;
            std::string type = citem.get<std::string>("type");
            ContextPtr ctx = ContextFactory::create(type);
            if(ctx) {
                ctx->load(citem);
                setContext(kv.first.data() , ctx);
            } else {
                GLOB_LOGGER("json.ctx");
                BOOST_LOG_SEV(logger, Error) << " Failed to find an appropriate factory for context type: " << type;
            }
        }
    }
}

