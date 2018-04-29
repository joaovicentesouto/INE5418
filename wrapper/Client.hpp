#include "TCPTypes.hpp"

namespace connection
{
    class Client
    {
    public:
        
        // Class constructors
        Client() = delete;

        Client(const Client&) = delete;
        Client& operator=(const Client&) = delete;

        Client(Client&&) = default;
        Client& operator=(Client&&) = default;

        Client(const string_type& server_ip, const string_type& server_port);

        // Class member functions
        virtual void write(const string_type& msg) = 0;
        virtual const string_type&& read() = 0;

    private:
        address_type    m_server_address;
        socket_type     m_socket;
        io_service_type m_service;
    };
}   // namespace connection