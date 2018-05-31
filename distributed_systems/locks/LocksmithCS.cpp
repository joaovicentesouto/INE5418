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

    sleep(1);
}

void LocksmithCS::llock()
{
    resolver_type resolver(m_client_service);

    std::cout << "++ 1 Cliente chama connect\n";
    type::network::connect(m_client, resolver.resolve(query_type(m_server_name, "62000")));

    std::cout << "++ 2 Cliente envia requisição\n";
    type::network::write(m_client, type::network::buffer(m_hostname, m_hostname.size()));

    std::cout << "++ 3 Cliente espera confirmação\n";
    char reply[m_hostname.size()];
    type::network::read(m_client, type::network::buffer(reply, m_hostname.size()));

    std::cout << "++ 4 Cliente tem a chave e vai fazer o que quiser\n\n";
}

void LocksmithCS::lunlock()
{
    std::cout << "++ 5 Cliente devolve a chave\n";
    type::network::write(m_client, type::network::buffer(m_hostname, m_hostname.size()));

    m_client.close();
    std::cout << "++ 6 Cliente fecha conexão e volta ao estado normal\n";
}

void LocksmithCS::server_rises()
{
    using mutex_type = std::mutex;

    std::function<void(socket_type, mutex_type*)> session = [](socket_type sock, mutex_type* mutex)
    {
        try
        {
            char requester[25];

            //! Recebe nome do requisitante
            boost::system::error_code error;

            size_t length = sock.read_some(boost::asio::buffer(requester), error);
            std::cout << "-- 3 Servidor recebe requisicao de: " << requester << std::endl;

            if (error == boost::asio::error::eof) //! precisa desse primeiro erro?
                return; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.

            mutex->lock();

            //! Envia chave... (nome do requisitante)
            std::cout << "-- 4 Servidor responde, entregando a chave\n";
            boost::asio::write(sock, boost::asio::buffer(requester, length));

            //! Recebe chave devolta... (nome do requisitante)
            std::cout << "-- 5 Servidor aguarda retorno da chave\n";
            length = sock.read_some(boost::asio::buffer(requester), error);

            std::cout << "-- 6 Servidor recebe chave e continua\n";

            if (error == boost::asio::error::eof)
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
            std::cerr << "Exception in thread: " << e.what() << "\n";
        }
    };

    mutex_type mutex;
    io_service_type server_service;
    type::tcp::acceptor_type acceptor(server_service, type::tcp::address_type(type::ip::tcp::v4(), 62000));
    std::cout << "-- 1 Servidor criado\n";

    while (true)
    {
        socket_type sock(server_service);
        acceptor.accept(sock);
        std::cout << "-- 2 Servidor aceita conexão\n";
        std::thread(session, std::move(sock), &mutex).detach();
    }
}

} // namespace lock
