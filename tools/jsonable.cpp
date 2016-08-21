#include <tools/jsonable.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>

Jsonable::Jsonable() {}
Jsonable::~Jsonable(){}

void Jsonable::save(std::string & str) const {
    boost::property_tree::ptree root;
    save(root);
    
    std::stringstream ss;
    write_json(ss, root);
    str = ss.str();
    
}

void Jsonable::load(const std::string & str) {
    boost::property_tree::ptree root;
    std::stringstream ss(str);
    load_json(ss, root);
}
