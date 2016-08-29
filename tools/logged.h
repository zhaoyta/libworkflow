#ifndef __LOGGED_H_
#define __LOGGED_H_

#include <tools/jsonable.h>
#include <tools/defines.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>


BEGIN_WEAK_ENUM_DECL(LogSeverity) {
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Critical
};
END_ENUM_DECL(LogSeverity, Info, "Info");


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
  
    boost::log::sources::severity_logger<ELogSeverity> logger;
};

#endif //__LOGGED_H_