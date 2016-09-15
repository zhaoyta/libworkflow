#include <tools/jsonable.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <exception>
#include <tools/logged.h>

Jsonable::Jsonable() {}
Jsonable::~Jsonable(){}

void Jsonable::str_save(std::string & str) const {
    boost::property_tree::ptree root;
    save(root);
    
    std::stringstream ss;
    write_json(ss, root);
    str = ss.str();
    
}

bool Jsonable::str_load(const std::string & str) {
    if(str.empty())
        return false;
    
    try {
        boost::property_tree::ptree root;
        std::stringstream ss(str);
        read_json(ss, root);
        load(root);
    } catch (std::exception const& e) {
        GLOB_LOGGER("json");
        BOOST_LOG_SEV(logger, Error) << "Failed to read json: " << e.what();
        BOOST_LOG_SEV(logger, Error) << "Incriminated json: " << str;
        return false;
    }
    return true;
}
