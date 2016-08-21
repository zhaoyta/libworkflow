#ifndef __CONTEXT_H_
#define __CONTEXT_H_
#include <tools/jsonable.h>
#include <tools/defines.h>
#pragma GCC visibility push(default)

SHARED_PTR(Context);
/**
 Stores data for Actions usage.
 */
class Context : public Jsonable {
    const std::string type;
public:
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
    ListCtx();
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

OSTREAM_HELPER_DECL(Context);

#pragma GCC visibility pop

#endif // __CONTEXT_H_