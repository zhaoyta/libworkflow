#ifndef __HTTP_INPUT_H_
#define __HTTP_INPUT_H_

#include <service/input.h>

/**
 HTTPInput: Will open a web socket. All request will be redirected to HTTP Client. 
 
 */
class HTTPInput: public Input {
public:
    HTTPInput(const std::string & name, bool delay_start = false);
    virtual ~HTTPInput();
};

#endif // __HTTP_INPUT_H_