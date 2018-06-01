#include "LocksmithCS.hpp"

namespace locks
{

LocksmithCS::LocksmithCS() :
    m_hostname(type::ip::host_name()),
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

    type::network::connect(m_client, resolver.resolve(query_type(m_server_name, "62000")));

    char request[100];
    strcpy(request, m_hostname.c_str());
    type::network::write(m_client, type::network::buffer(request, 100));

    char reply[100];
    type::network::read(m_client, type::network::buffer(reply, 100));
}

void LocksmithCS::lunlock()
{
    char return_key[100];
    strcpy(return_key, m_hostname.c_str());
    type::network::write(m_client, type::network::buffer(return_key, 100));

    m_client.close();
}

void LocksmithCS::server_rises()
{
    using mutex_type = std::mutex;

    std::function<void(socket_type, mutex_type*)> session = [](socket_type sock, mutex_type* mutex)
    {
        try
        {
            char requester[100];

            //! Recebe nome do requisitante
            boost::system::error_code error;

            size_t length = sock.read_some(type::network::buffer(requester), error);

            if (error == type::network::error::eof) //! precisa desse primeiro erro?
                return; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.

            mutex->lock();

            //! Envia chave... (nome do requisitante)
            type::network::write(sock, type::network::buffer(requester, length));

            //! Recebe chave devolta... (nome do requisitante)
            length = sock.read_some(type::network::buffer(requester), error);

            if (error == type::network::error::eof)
            {
                mutex->unlock();
                return; // Connection closed cleanly by peer.
            }
            else if (error)
            {
                mutex->unlock();
                throw boost::system::system_error(error); // Some other error.
            }

            mutex->unlock();
        }
        catch (std::exception& e)
        {
            mutex->unlock();
            std::cerr << "Exception in thread: " << e.what() << "\n" << std::flush;
        }
    };

    mutex_type mutex;
    io_service_type server_service;
    type::tcp::acceptor_type acceptor(server_service, type::tcp::address_type(type::ip::tcp::v4(), 62000));

    while (true)
    {
        socket_type sock(server_service);
        acceptor.accept(sock);
        std::thread(session, std::move(sock), &mutex).detach();
    }
}

} // namespace lock
