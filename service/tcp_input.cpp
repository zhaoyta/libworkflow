#include <service/tcp_input.h>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <service/tcp_client.h>
#include <service/client_manager.h>

using boost::asio::ip::tcp;

TCPInput::TCPInput(const std::string & name,const std::string & address, uint32_t port, bool delay):
    Input(name, delay),
    address(address),
    port(port),
    acceptor(*getIOService(), tcp::endpoint(tcp::v4(), port)),
    socket(*getIOService()) {}

TCPInput::~TCPInput() {}

void TCPInput::started() {
    auto client = TCPClientPtr(new TCPClient(getIOService()));
    acceptor.async_accept(client->getSocket(),
                           boost::bind(&TCPInput::handle_accept, this, client, boost::asio::placeholders::error));
}

void TCPInput::handle_accept(TCPClientPtr client, const boost::system::error_code& error)
{
    if (!error)
    {
        ClientManager::getInstance()->addClient(client);
        
        auto clt = TCPClientPtr(new TCPClient(getIOService()));
        acceptor.async_accept(clt->getSocket(),
                              boost::bind(&TCPInput::handle_accept, this, clt, boost::asio::placeholders::error));
    } else {
        BOOST_LOG_SEV(logger, Error) << this << " Failed to accept new connection: " << error.message();
    }
}
