#include "LocksmithCS.hpp"

namespace locks
{

LocksmithCS::LocksmithCS() :
    m_hostname(string_type("container") + std::getenv("ID")),
    m_server_name(std::getenv("SERVER_NAME")),
    m_client_service(),
    m_client(m_client_service)
{
    std::cout << "Initiating client/server algorithm." << std::flush;

    if (m_hostname == m_server_name)
        std::thread(&LocksmithCS::server_rises, this).detach();
}

void LocksmithCS::lock()
{
    resolver_type resolver(m_client_service);

    //! Creates connection
    type::network::connect(m_client, resolver.resolve(query_type(m_server_name, "62000")));

    char request[100];
    strcpy(request, m_hostname.c_str());

    //! Requests permission
    type::network::write(m_client, type::network::buffer(request, 100));

    //! Waits permission
    type::network::read(m_client, type::network::buffer(request, 100));
}

void LocksmithCS::unlock()
{
    char return_key[100];
    strcpy(return_key, m_hostname.c_str());
    type::network::write(m_client, type::network::buffer(return_key, 100));

    //! Close connection
    m_client.close();
}

void LocksmithCS::server_rises()
{
    char requester[100];
    type::error_type error;
    io_service_type server_service;
    type::tcp::acceptor_type acceptor(server_service, type::tcp::address_type(type::ip::tcp::v4(), 62000));

    while (true)
    {
        socket_type socket(server_service);
        acceptor.accept(socket);

        try
        {
            //! Recieves permission
            socket.read_some(type::network::buffer(requester), error);

            std::cout << "\n- Server: Request by:        " << requester << std::endl << std::flush;
            std::cout << "- Server: Allows access to:  " << requester << std::endl << std::flush;

            //! Sends permission
            type::network::write(socket, type::network::buffer(requester, 100));

            //! Waits confirmation
            socket.read_some(type::network::buffer(requester), error);

            std::cout << "- Server: Exit confirmation: " << requester << std::endl << std::flush;
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception in thread: " << e.what() << "\n" << std::flush;
        }
    }
}

} //! namespace lock
