#include "Server.hpp"

namespace connection
{
    Server::Server(port_type port) :
        m_address(protocol_type::v4(), port)
    {

    }

}   // namespace connection