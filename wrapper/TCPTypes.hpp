#include <string>
#include <boost/asio.hpp>

namespace connection
{
    // Namespace member types
    using string_type     = std::string;
    using port_type       = const unsigned short;
    
    using io_service_type = boost::asio::io_service;
    using protocol_type   = boost::asio::ip::tcp;

    using address_type    = protocol_type::endpoint;
    using resolver_type   = protocol_type::resolver;
    using query_type      = resolver_type::query;
    using acceptor_type   = protocol_type::acceptor;
    using socket_type     = protocol_type::socket;

    // Namespace member function
    //using buffer          = boost::asio::buffer;
    //using write           = boost::asio::write;
    //using read            = boost::asio::read;

}   // namespace connection
