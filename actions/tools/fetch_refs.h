#ifndef __FETCH_REFS_H_
#define __FETCH_REFS_H_

#include <core/action.h>

/**
    FetchRefs will search through all stored Actions, Contexts and Workflows
    And fill a RefsStorage with all this. It's a map to be used to build new workflows.
 */
class FetchRefs: public Action {
public:
    FetchRefs();
    virtual ~FetchRefs();
    
    Result perform(SessionPtr) const override;
};

OSTREAM_HELPER_DECL(FetchRefs);

#endif // __FETCH_REFS_H_