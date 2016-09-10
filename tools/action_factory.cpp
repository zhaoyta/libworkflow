#include <tools/action_factory.h>

std::map<std::string, _ActionBuilder *> ActionFactory::builders;

void ActionFactory::registerAction(_ActionBuilder * ab) {
    if(ab and not ab->getName().empty()) {
        builders[ab->getName()] = ab;
    } else {
        // Well should log we can't store an empty builder.
    }
}

ActionPtr ActionFactory::create(const std::string & name) {
    if(builders.count(name) > 0) {
        return builders[name]->create();
    }
    return ActionPtr();
}

std::map<std::string, _ActionBuilder *> ActionFactory::getBuilders() {
    return builders;
}


ActionPtr _ActionBuilder::create() {
    return ActionPtr();
}

std::string _ActionBuilder::getName() {
    auto act = create();
    if(act)
        return act->getName();
    return "";
}

