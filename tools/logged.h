#ifndef __LOGGED_H_
#define __LOGGED_H_

#include <tools/jsonable.h>
#include <tools/defines.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/utility/formatting_ostream.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

namespace logging = boost::log;
namespace expr = boost::log::expressions;


BEGIN_WEAK_ENUM_DECL(LogSeverity) {
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Critical
};
END_ENUM_DECL(LogSeverity, Info, "Info");

#define GLOB_LOGGER(ns) \
    boost::log::sources::severity_logger<ELogSeverity> logger; \
    logger.add_attribute("Namespace", \
                         boost::log::attributes::constant< std::string >(ns)); \


/**
 Add appropriate logging informations. use in conjunction with boost log.
 
 BOOST_LOG_SEV(logger, Info) << "A regular message";
 BOOST_LOG_SEV(logger, Warn) << "Something bad is going on but I can handle it";
 BOOST_LOG_SEV(logger, Critical) << "Everything crumbles, shoot me now!";
 */
class Logged {
    boost::log::attributes::mutable_constant< std::string > attr;

    static struct Configuration: public Jsonable {
        Configuration();
        
        std::string file;
        bool output_to_console;
        
        bool show_thread;
        
        ELogSeverity severity;
        
        void save(boost::property_tree::ptree & root) const override;
        void load(const boost::property_tree::ptree & root) override;
    } configuration;
public:
    Logged(const std::string & ns);
    virtual ~Logged();
    
    void setNamespace(const std::string & ns);
    
    //! seek provided @a filename to load log configuration
    static void loadConfiguration(const std::string & filename);
  

protected:
    
    mutable boost::log::sources::severity_logger<ELogSeverity> logger;

};

#endif //__LOGGED_H_