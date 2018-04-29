#include "TCPTypes.hpp"

namespace connection
{
    class Server
    {
    public:

        // Class constructors
        Server() = delete;

        Server(const Server&) = delete;
        Server& operator=(const Server&) = delete;

        Server(Server&&) = default;
        Server& operator=(Server&&) = default;

        Server(port_type port);

        // Class member functions
        virtual void run() = 0;

    private:
        address_type    m_address;
        io_service_type m_service;
    };
}   // namespace connection
