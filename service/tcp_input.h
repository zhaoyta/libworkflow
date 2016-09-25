#ifndef __TCP_INPUT_H_
#define __TCP_INPUT_H_

#include <service/input.h>

/**
 TCP open a server socket, and accept connection from outside, spawn new TCP Client. 
  */
class TCPInput: public Input {
public:
    TCPInput(const std::string & name, bool delay_start = false);
    virtual ~TCPInput();
};

#endif // __TCP_INPUT_H_