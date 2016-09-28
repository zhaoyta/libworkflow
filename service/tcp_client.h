#ifndef __TCP_CLIENT_H_
#define __TCP_CLIENT_H_

#include <service/client.h>
#include <tools/defines.h>
#include <boost/asio.hpp>

SHARED_PTR(TCPClient);
typedef boost::shared_ptr<boost::asio::io_service> IOServicePtr;


/**
 TCP Client is spawned from TCP Input. It represent an established tcp connection.
 */
class TCPClient: public Client {
    IOServicePtr service;
    boost::asio::ip::tcp::socket socket;
    enum { max_length = 1024 };
    char data_[max_length];
    std::stringstream buff;
    size_t expected_size;
    std::string address;
    int32_t port;
    
public:
    TCPClient(IOServicePtr);
    virtual ~TCPClient();
    
    boost::asio::ip::tcp::socket & getSocket();
    
    void setConnectionDetails(const std::string & address, int32_t port);
    
    void connect() override;
    void disconnect() override;
    
    void startRead();
    
protected:
    void handle_read_head(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_read_body(const boost::system::error_code& error, size_t bytes_transferred);

    void handle_write(const boost::system::error_code& error);
    
    
    //! do the actual sending.
    bool doSendRequest(RequestPtr) override;
    
    //! check ability to handle this request.
    //! @return true by default
    bool isReceivedRequestAllowed(RequestPtr, ErrorReport &) override;
    
    //! check ability to send this request
    //! @return false by default.
    bool canSendRequest(RequestPtr, ErrorReport &) override;
};

#endif // __TCP_CLIENT_H_