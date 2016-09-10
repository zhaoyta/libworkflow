#include <tools/context_factory.h>
#include <tools/logged.h>


std::map<std::string, _ContextBuilder * > ContextFactory::builders;
ContextFactory::ContextFactory() {}



void ContextFactory::registerContext(_ContextBuilder * cbuild) {
    if(cbuild ){
        if(not cbuild->getName().empty()) {
        builders[cbuild->getName()] = cbuild;
        } else {
            GLOB_LOGGER("ctx.builder");
            BOOST_LOG_SEV(logger, Error) << " provided builder is invalid. ";
        }
    } else {
        GLOB_LOGGER("ctx.builder");
        BOOST_LOG_SEV(logger, Error) << " provided builder is NULL. ";
    }
}


std::map<std::string, _ContextBuilder * > ContextFactory::getBuilders() {
    return builders;
}

ContextPtr ContextFactory::create(const std::string & type) {
    if(builders.count(type) > 0) {
        return builders[type]->create();
    }
    return ContextPtr();
}

ContextPtr _ContextBuilder::create() {
    return ContextPtr();
}

std::string _ContextBuilder::getName() {
    auto ctx = create();
    if(ctx)
        return ctx->getType();
    return "";
}