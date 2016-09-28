#ifndef __TCP_INPUT_H_
#define __TCP_INPUT_H_

#include <service/input.h>
#include <boost/asio.hpp>
#include <tools/defines.h>
SHARED_PTR(TCPInput);
SHARED_PTR(TCPClient);

/**
 TCP open a server socket, and accept connection from outside, spawn new TCP Client. 
  */
class TCPInput: public Input {
    std::string address;
    uint32_t port;
    boost::asio::ip::tcp::socket socket;
    boost::asio::ip::tcp::acceptor acceptor;

public:
    TCPInput(const std::string & name, const std::string & address, uint32_t port, bool delay_start = false);
    virtual ~TCPInput();

protected:
    void started() override;
    void handle_accept(TCPClientPtr client, const boost::system::error_code& error);

};

#endif // __TCP_INPUT_H_