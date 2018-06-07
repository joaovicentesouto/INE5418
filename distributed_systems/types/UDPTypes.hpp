#ifndef UDP_TYPES_HPP
#define UDP_TYPES_HPP

#include <set>
#include "BasicTypes.hpp"

namespace type
{
namespace udp
{
    // Namespace member types
    using port_type          = const unsigned short;
    
    using io_service_type    = network::io_service;
    using protocol_type      = ip::udp;

    using endpoint_type      = protocol_type::endpoint;
    using resolver_type      = protocol_type::resolver;
    using query_type         = resolver_type::query;
    // using acceptor_type   = protocol_type::acceptor; IS IT NEED?
    using socket_type        = protocol_type::socket;

    using endpoints_set_type = std::set<endpoint_type>;

    // Namespace member function
    //using buffer          = boost::asio::buffer;
    //using write           = boost::asio::write;
    //using read            = boost::asio::read;

}   // namespace udp
}   // namespace types

#endif  // UDP_TYPES_HPP