#include "LocksmithCS.hpp"

namespace locks
{

LocksmithCS::LocksmithCS() :
    m_hostname(string_type("container") + std::getenv("ID")),
    m_server_name(std::getenv("SERVER_NAME")),
    m_client_service(),
    m_client(m_client_service)
{
    if (m_hostname == m_server_name)
        std::thread(&LocksmithCS::server_rises, this).detach();
}

void LocksmithCS::llock()
{
    resolver_type resolver(m_client_service);

    // Cria conexão com o servidor
    type::network::connect(m_client, resolver.resolve(query_type(m_server_name, "62000")));

    char request[100];
    strcpy(request, m_hostname.c_str());

    // Realiza requisição
    type::network::write(m_client, type::network::buffer(request, 100));

    // Espera permissão
    type::network::read(m_client, type::network::buffer(request, 100));
}

void LocksmithCS::lunlock()
{
    char return_key[100];
    strcpy(return_key, m_hostname.c_str());
    type::network::write(m_client, type::network::buffer(return_key, 100));

    // Fecha conexão
    m_client.close();
}

void LocksmithCS::server_rises()
{
    char requester[100];
    io_service_type server_service;
    type::tcp::acceptor_type acceptor(server_service, type::tcp::address_type(type::ip::tcp::v4(), 62000));

    while (true)
    {
        socket_type sock(server_service);
        acceptor.accept(sock);

        try
        {
            type::error_type error;

            // Recebe nome do requisitante
            size_t length = sock.read_some(type::network::buffer(requester), error);

            std::cout << " *** Recebe requisição de: " << requester << std::endl << std::flush;

            if (error == type::network::error::eof) // precisa desse primeiro erro?
                return; //! Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); //! Some other error.

            std::cout << " *** Envia chave para: " << requester << std::endl << std::flush;

            // Envia chave... (nome do requisitante)
            type::network::write(sock, type::network::buffer(requester, length));

            // Recebe chave devolta... (nome do requisitante)
            length = sock.read_some(type::network::buffer(requester), error);

            std::cout << " *** Recebe chave de: " << requester << std::endl << std::flush;

            if (error == type::network::error::eof)
                return; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.

        }
        catch (std::exception& e)
        {
            std::cerr << "Exception in thread: " << e.what() << "\n" << std::flush;
        }
    }
}

} // namespace lock
