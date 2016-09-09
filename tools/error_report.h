#ifndef __ERROR_REPORT_H_
#define __ERROR_REPORT_H_

#include <tools/jsonable.h>
#include <tools/defines.h>
#include <core/target.h>
#include <vector>
#include <boost/uuid/uuid.hpp>

#pragma GCC visibility push(default)

SHARED_PTR(ErrorReport);

/**
    This is like a callback stack, it allow to store an error, and trace execution stack. 
 
 */
class ErrorReport: public Jsonable, public boost::enable_shared_from_this<ErrorReport> {
    //! Whom issued this error.
    Target target;
    //! Reason, human readble
    std::string error_message;
    //! some key for eventual post processing.
    std::string error_key;
    
    ErrorReportPtr parent;
public:
    ErrorReport();
    ErrorReport(const ErrorReport & );
    //! Create an error report.
    ErrorReport(const Target & target, const std::string & error_key, const std::string & error_msg);
    //! This is rethrow version, add provided error report to the stack.
    ErrorReport(const Target & target, ErrorReportPtr, const std::string & error_key = "", const std::string & error_msg = "");
    virtual ~ErrorReport();
    
    //! set error content.
    void setError(const std::string & error_key, const std::string & error_message);
    
    //! An error is concidered unset if no error_key has been provided and no parent have been assigned.
    bool isSet() const;
    
    
    //! @return topmost error message.
    const std::string & getErrorMessage() ;
    //! @return topmost error key
    const std::string & getErrorKey() ;
    //! @return ErrorReport parent.
    const ErrorReportPtr getParent() ;
    //! @return a vector of all error report ancestry of this error.
    const std::vector<ErrorReportPtr> getAncestry();
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
};


#pragma GCC visibility pop

#endif // __ERROR_REPORT_H_