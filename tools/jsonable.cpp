#include <tools/jsonable.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>

Jsonable::Jsonable() {}
Jsonable::~Jsonable(){}

void Jsonable::str_save(std::string & str) const {
    boost::property_tree::ptree root;
    save(root);
    
    std::stringstream ss;
    write_json(ss, root);
    str = ss.str();
    
}

void Jsonable::str_load(const std::string & str) {
    if(str.empty())
        return;
    boost::property_tree::ptree root;
    std::stringstream ss(str);
    read_json(ss, root);
    load(root);
}
