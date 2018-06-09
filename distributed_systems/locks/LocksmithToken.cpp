#include "LocksmithToken.hpp"

namespace locks
{

LocksmithToken::LocksmithToken() :
    m_containers_amount(std::stoi(std::getenv("CONTAINERS_AMOUNT"))),
    m_next_name(string_type("container") + std::to_string((std::stoi(std::getenv("ID"))+1) % std::stoi(std::getenv("CONTAINERS_AMOUNT")))),
    m_hostname(string_type("container") + std::getenv("ID"))
{
    type::thread_type(&LocksmithToken::ring_algorithm, this).detach();

    sleep(1);
}

void LocksmithToken::lock()
{
    m_standby_mutex.lock();

    //! Wait for permission.
    m_critical_mutex.lock();
}

void LocksmithToken::unlock()
{
    //! Frees the ring thread.
    m_critical_mutex.unlock();
    m_standby_mutex.unlock();
}

void LocksmithToken::ring_algorithm()
{
    m_critical_mutex.lock();
    
    char message[100];
    type::error_type error;
    type::network::io_service udp_server;

    std::cout << "1\n" << std::flush;
    type::udp::socket_type socket(udp_server, protocol_type::endpoint(type::ip::udp::v4(), 62000));
    std::cout << "1\n" << std::flush;

    resolver_type resolver(udp_server);
    type::udp::endpoint_type predecessor;
    std::cout << "r 2\n" << std::flush;
    type::udp::endpoint_type next_address = *resolver.resolve(query_type(type::ip::udp::v4(), m_next_name, "62000"));
    std::cout << "r 2\n" << std::flush;

    //! Last container starts sending token.
    if (m_next_name == "container0")
    {
        std::cout << "s 3\n" << std::flush;
        socket.send_to(type::network::buffer(message), next_address, 0, error);
        std::cout << "s 3\n" << std::flush;
    }

    while (true)
    {
        //! Wait token.
        std::cout << "rr 4\n" << std::flush;
        socket.receive_from(type::network::buffer(message), predecessor, 0, error);
        std::cout << "rr 4\n" << std::flush;

        //! Releases thread main.
        m_critical_mutex.unlock();
        m_standby_mutex.lock();

        m_critical_mutex.lock();
        m_standby_mutex.unlock();

        //! Sends token.
        strcpy(message, m_hostname.c_str());
        std::cout << "ss 5\n" << std::flush;
        socket.send_to(type::network::buffer(message), next_address, 0, error);
        std::cout << "ss 5\n" << std::flush;
    }
}

} // namespace lock