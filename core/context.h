#ifndef __CONTEXT_H_
#define __CONTEXT_H_
#include <tools/jsonable.h>
#include <tools/defines.h>
#pragma GCC visibility push(default)

SHARED_PTR(Context);
class Context : public Jsonable {
public:
    Context();
    virtual ~Context();
    
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
    
};

OSTREAM_HELPER_DECL(Context);

#pragma GCC visibility pop

#endif // __CONTEXT_H_