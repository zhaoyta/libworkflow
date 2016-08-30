#include <tools/logged.h>
#include <string>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/utility/formatting_ostream.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/utility/empty_deleter.hpp>
#include <iomanip>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/utility/value_ref.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/trivial.hpp>



namespace logging = boost::log;
namespace expr = boost::log::expressions;


Logged::Configuration Logged::configuration;

namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

typedef sinks::synchronous_sink< sinks::text_ostream_backend > sink_t;

BEGIN_ENUM_IMPL(LogSeverity) {
    {"Trace",(uint32_t)ELogSeverity::Trace },
    {"Debug",(uint32_t)ELogSeverity::Debug },
    {"Info",(uint32_t)ELogSeverity::Info },
    {"Warn",(uint32_t)ELogSeverity::Warn },
    {"Error",(uint32_t)ELogSeverity::Error},
    {"Critical",(uint32_t)ELogSeverity::Critical }
};
END_ENUM_IMPL(LogSeverity);


BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", ELogSeverity)
BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Namespace", std::string)

Logged::Configuration::Configuration():
    file(""),
    output_to_console(true),
    show_thread(true),
    severity(Trace){
}

void Logged::Configuration::save(boost::property_tree::ptree & root) const {
    
}

void Logged::Configuration::load(const boost::property_tree::ptree & root) {
    
}

Logged::Logged(const std::string & ns): attr(ns) {
    logger.add_attribute("Namespace", attr);
    setNamespace(ns);
}

Logged::~Logged() {
    
}

void Logged::setNamespace(const std::string & nas) {
    attr.set(nas);
}

void Logged::loadConfiguration(const std::string & filename) {
    if(not configuration.file.empty()) {
        boost::log::add_file_log(configuration.file);
    }
    switch(configuration.severity) {
        case Info:
            boost::log::core::get()->set_filter
            (
             severity >= ELogSeverity::Info
             );
            break;
        default:
            boost::log::core::get()->set_filter
            (
             severity >= ELogSeverity::Trace
             );
            break;
    }
    
    
    // Add attributes
    boost::log::add_common_attributes();

    
    logging::core::get()->add_global_attribute(
                                               "ThreadID",
                                               boost::log::attributes::current_thread_id());
    
    boost::log::formatter fmt = boost::log::expressions::stream
    << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
    << " [" << expr::attr< boost::log::attributes::current_thread_id::value_type>("ThreadID") << "] "
    << " <" << std::setw(6) << std::setfill(' ') << severity << "> "
    << std::setw(10) << std::setfill(' ')
    << expr::attr<std::string>("Namespace") << " "
    << boost::log::expressions::smessage;
    
    
    
    boost::shared_ptr< logging::core > core = logging::core::get();
    
    // Create a backend and initialize it with a stream
    boost::shared_ptr< sinks::text_ostream_backend > backend =
    boost::make_shared< sinks::text_ostream_backend >();
    backend->add_stream(
                        boost::shared_ptr< std::ostream >(&std::clog, boost::empty_deleter()));
    
    // Wrap it into the frontend and register in the core
    boost::shared_ptr< sink_t > sink(new sink_t(backend));
    sink->set_formatter(fmt);
    core->add_sink(sink);

    
}

