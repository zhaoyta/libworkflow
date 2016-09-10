#ifndef __REFS_STORAGE_CTX_H_
#define __REFS_STORAGE_CTX_H_

#include <core/context.h>
#include <map>
SHARED_PTR(RefsStorageCtx);

/**
    This may contains Actions, Contexts and workflow definitions.
 */
class RefsStorageCtx: public Context {
    std::map<std::string, std::string> actions;
    std::map<std::string, std::string> contexts;
    std::map<std::string, std::map<std::string, std::string> > workflows;
    
public:
    RefsStorageCtx();
    virtual ~RefsStorageCtx();
    
    const std::map<std::string, std::string> & getActions() const;
    const std::map<std::string, std::string> & getContexts() const ;
    const std::map<std::string, std::map<std::string, std::string> > & getWorkfows() const;
    
    void setActions(const std::map<std::string, std::string> & );
    void setContexts(const std::map<std::string, std::string> & );
    void setWorkflows(const std::map<std::string, std::map<std::string,std::string> > & );
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
    
};
#endif // __REFS_STORAGE_CTX_H_