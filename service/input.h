#ifndef __INPUT_H_
#define __INPUT_H_

#include <tools/defines.h>
#include <tools/active_object.h>
SHARED_PTR(Input);

/**
    Inputs are "clients generator". Mostly, we'll have a nice TCP Input, that will stand for a TCP Socket server. Or an UDP one, or a HTTP one. '
 */
class Input: public ActiveObject {
public:
    Input(const std::string &name,  bool delay_start = false);
    virtual ~Input();
};

#endif // __INPUT_H_