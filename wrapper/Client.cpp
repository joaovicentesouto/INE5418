#include "Client.hpp"

namespace connection
{

    Client::Client(const string_type& server_ip, const string_type& server_port) :
        m_service{},
        m_socket{m_service},
        m_server_address{
            *resolver_type{m_service}.resolve(resolver_type::query(server_ip, server_port))
        }
    {

    }
}   // namespace connection