#include <contexts/refs_storage_ctx.h>

RefsStorageCtx::RefsStorageCtx(): Context("RefsStorageCtx") {}
RefsStorageCtx::~RefsStorageCtx() {
    
}

const std::map<std::string, std::string> & RefsStorageCtx::getActions() const{
    return actions;
}

const std::map<std::string, std::string> & RefsStorageCtx::getContexts() const  {
    return contexts;
}

const std::map<std::string, std::map<std::string, std::string> > & RefsStorageCtx::getWorkfows() const {
    return workflows;
}

void RefsStorageCtx::setActions(const std::map<std::string, std::string> & a){
    actions = a ;
}

void RefsStorageCtx::setContexts(const std::map<std::string, std::string> & c){
    contexts = c;
}

void RefsStorageCtx::setWorkflows(const std::map<std::string, std::map<std::string,std::string> > & w) {
    workflows = w;
}

void RefsStorageCtx::save(boost::property_tree::ptree & root) const {
    boost::property_tree::ptree pactions;
    for(const auto & kv: actions) {
        pactions.put(kv.first, kv.second);
    }
    root.add_child("actions", pactions);
    
    boost::property_tree::ptree pcontexts;
    for(const auto & kv: contexts)
        pcontexts.put(kv.first, kv.second);
    root.add_child("contexts", pcontexts);
    
    boost::property_tree::ptree pworkflows;
    for(const auto & kv: workflows) {
        boost::property_tree::ptree pcontroller;
        for(const auto & kw: kv.second)
            pcontroller.put(kw.first, kw.second);
        pworkflows.add_child(kv.first, pcontroller);
    }
    root.add_child("workflows", pworkflows);
    
}

void RefsStorageCtx::load(const boost::property_tree::ptree & root) {
    auto oactions = root.get_child_optional("actions");
    if(oactions)
        for(const auto & kv: *oactions)
            actions[kv.first.data()] = kv.second.data();
    
    auto ocontexts = root.get_child_optional("contexts");
    if(ocontexts)
        for(const auto & kv: *ocontexts)
            contexts[kv.first.data()] = kv.second.data();
    
    
    auto oworkflows = root.get_child_optional("workflows");
    if(oworkflows) {
        for(const auto & kv: *oworkflows) {
            for(const auto & kw: kv.second) {
            workflows[kv.first.data()][kw.first.data()] = kw.second.data();
            }
        }
    }
}