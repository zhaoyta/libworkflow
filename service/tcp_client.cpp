#include <service/tcp_client.h>
#include <sstream>
#include <iomanip>
#include <core/request.h>

TCPClient::TCPClient(IOServicePtr s): Client(), service(s), socket(*s) {}
TCPClient::~TCPClient() {}

boost::asio::ip::tcp::socket & TCPClient::getSocket() {
    return socket;
}

void TCPClient::startRead() {
    // Check socket is open beforehand :)
    socket.async_read_some(boost::asio::buffer(data_, 10),
                            boost::bind(&TCPClient::handle_read_head, this,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));
}

void TCPClient::handle_read_head(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        data_[10] = '\0';
        std::stringstream str(data_);
        expected_size = 0;
        str >> expected_size;
        
        socket.async_read_some(boost::asio::buffer(data_, max_length -1),
                               boost::bind(&TCPClient::handle_read_body, this,
                                           boost::asio::placeholders::error,
                                           boost::asio::placeholders::bytes_transferred));
        // when finished: receiveNewRequest(RequestPtr, const ErrorReport &)
    }
    else
    {
        BOOST_LOG_SEV(logger, Error) << this << " Handle read failed: " << error.message();
    }
}

void TCPClient::handle_read_body(const boost::system::error_code& error, size_t bytes_transferred)
{
    expected_size -= bytes_transferred;
    if (!error and expected_size == 0)
    {
        auto req = RequestPtr(new Request());
        req->str_load(buff.str());
        
        ErrorReport er;
        if(not receiveNewRequest(req, er)) {
            BOOST_LOG_SEV(logger, Error) << this << "Failed to read a Request and publish it: " << er.getErrorMessage();
        }
        startRead();
    } else if(!error) {
        data_[bytes_transferred] = '\0';
        buff << data_;
    } else {
        BOOST_LOG_SEV(logger, Error) << this << " Handle read failed: " << error.message();
    }
}

void TCPClient::handle_write(const boost::system::error_code& error)
{
    if (error)
    {
        BOOST_LOG_SEV(logger, Error) << this << " Handle write failed: " << error.message();
    }
}

bool TCPClient::doSendRequest(RequestPtr req) {
    std::string json;
    req->str_save(json);
    
    std::stringstream str;
    str << std::setw(10) << std::setfill('0') << json.length() << json;
    
    boost::asio::async_write(socket,
                             boost::asio::buffer(str.str(),str.str().length()),
                             boost::bind(&TCPClient::handle_write,
                                         this,
                                         boost::asio::placeholders::error));
    return true;
}

bool TCPClient::isReceivedRequestAllowed(RequestPtr , ErrorReport & ) {
    return true;
}

bool TCPClient::canSendRequest(RequestPtr req, ErrorReport & err) {
    if(req)
        return true;
    err.setError("tcp.send.empty", "Empty request can't be sent.");
    return false;
}
