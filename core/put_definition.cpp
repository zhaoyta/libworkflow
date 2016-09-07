#include <core/put_definition.h>
#include <tools/type_checker.h>

PutDefinition::PutDefinition() :
    mandatory(true),
    ignoreEmpty(false),
allowSkip(false) {}
PutDefinition::~PutDefinition() {}


bool PutDefinition::operator<(const PutDefinition & pd) const {
    return put_name < pd.put_name;
}

void PutDefinition::save(boost::property_tree::ptree & root) const {
    root.put("name", put_name);
    root.put("description", description);
    root.put("type", checker->generate()->getType());
    root.put("mandatory", mandatory);
    root.put("ignoreEmpty", ignoreEmpty);
    root.put("allowSkip", allowSkip);
}

void PutDefinition::load(const boost::property_tree::ptree & ) {
    //! This shouldn't get loaded.
}
