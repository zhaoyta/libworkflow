#ifndef __CONTEXT_H_
#define __CONTEXT_H_
#include <tools/jsonable.h>
#include <tools/defines.h>
#include <vector>
#pragma GCC visibility push(default)

SHARED_PTR(Context);
/**
 Stores data for Actions usage.
 Note: Dont forget to call Context::save(root) when overriding save ... otherwise json load wouldn't work. 
 */
class Context : public Jsonable {
    const std::string type;
public:
    Context();
    Context(const std::string & type);
    virtual ~Context();
    
    const std::string & getType() const;

    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
    
};

SHARED_PTR(ListCtx);

/**
    Stores other contexts as a List :)
 */
class ListCtx : public Context {
    std::vector<ContextPtr> contexts;
public:
    ListCtx(const std::string &);
    virtual ~ListCtx();
    
    template<class T>
    std::vector< boost::shared_ptr<T> > castVector() {
        std::vector< boost::shared_ptr<T> > res;
        for(auto c: contexts) {
            auto ctx = boost::dynamic_pointer_cast<T>(c);
            if(ctx)
                res.push_back(ctx);
        }
        return res;
    }
    
    bool isEmpty() const;
    
    void add(ContextPtr ctx);
    void clear();
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
};

SHARED_PTR(SkipCtx);
/** 
 This is a special kind of context, used to signify that bound actions shouldn't be executed.
 */
class SkipCtx : public Context {
public:
    SkipCtx();
    virtual ~SkipCtx();
};

SHARED_PTR(GroupedCtx);

/**
Grouped Ctx allow to store multiple contexts within one.
 */
class GroupedCtx: public Context {
    std::map<std::string, ContextPtr> contexts;
public:
    GroupedCtx();
    virtual ~GroupedCtx();
    
    //! @return numbers of contexts
    size_t size();
    
    ContextPtr getContext(const std::string &);
    template<class T>
    boost::shared_ptr<T> getCastedContext(const std::string & ctx) {
        return boost::dynamic_pointer_cast<T>(getContext(ctx));
    }
    
    void setContext(const std::string &, ContextPtr);
    
    std::map<std::string, ContextPtr> & getContexts();
    
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
    
    
};

OSTREAM_HELPER_DECL(Context);

#pragma GCC visibility pop

#endif // __CONTEXT_H_