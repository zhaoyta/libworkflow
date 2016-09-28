#include <service/tcp_client.h>
#include <sstream>
#include <iomanip>
#include <core/request.h>

using boost::asio::ip::tcp;

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
    if(not socket.is_open()) {
        BOOST_LOG_SEV(logger, Error) << this << req->logRequest() << " Can't send request, because client isn't connected.";
        
        return false;
    }
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

void TCPClient::setConnectionDetails(const std::string & add, int32_t p) {
    address = add;
    port = p;
}

void TCPClient::connect() {
    socket.get_io_service().dispatch(boost::bind<void>([&](){
        if(socket.is_open()) {
            BOOST_LOG_SEV(logger, Error) << this << "Can't connect with an already connected socket.";
            return;
        }
        
        tcp::resolver resolver(socket.get_io_service());
        tcp::resolver::query query(address, std::to_string(port));
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        boost::system::error_code ec;
    
        socket.connect(*endpoint_iterator, ec);
        if(not ec)
            return;
        
        BOOST_LOG_SEV(logger, Error) << " Socket failed to establish connection to: " << address << " due to: " << ec.message();
        
    }));
}

void TCPClient::disconnect() {
    socket.get_io_service().dispatch(boost::bind<void>([&](){
        if(socket.is_open()) {
            socket.cancel();
            socket.close();
        } else
            BOOST_LOG_SEV(logger, Error) << this << " Can't disconnect an already disconnected Socket.";
    }));
                                     
}
