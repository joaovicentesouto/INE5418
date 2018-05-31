#ifndef TCP_TYPES_HPP
#define TCP_TYPES_HPP

#include "BasicTypes.hpp"

namespace type
{
namespace tcp
{
    // Namespace member types
    using port_type       = const unsigned short;
    
    using io_service_type = network::io_service;
    using protocol_type   = ip::tcp;

    using address_type    = protocol_type::endpoint;
    using resolver_type   = protocol_type::resolver;
    using query_type      = resolver_type::query;
    using acceptor_type   = protocol_type::acceptor;
    using socket_type     = protocol_type::socket;

    // Namespace member function
    //using buffer          = boost::asio::buffer;
    //using write           = boost::asio::write;
    //using read            = boost::asio::read;

}   // namespace tcp
}   // namespace types

#endif  // TCP_TYPES_HPP
