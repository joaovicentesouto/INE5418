#include "LocksmithToken.hpp"

namespace locks
{

LocksmithToken::LocksmithToken() :
    m_next_name(string_type("container") + std::to_string((std::stoi(std::getenv("ID"))+1) % std::stoi(std::getenv("CONTAINERS_AMOUNT")))),
    m_hostname(string_type("container") + std::getenv("ID")),
    m_containers_amount(std::stoi(std::getenv("CONTAINERS_AMOUNT")))
{
    type::thread_type(&LocksmithToken::ring_algorithm, this).detach();
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
    type::udp::socket_type socket(udp_server, protocol_type::endpoint(type::ip::udp::v4(), 62000));

    //! Last container starts sending token.
    if (m_hostname == "container" + std::to_string(m_containers_amount-1))
    {
        resolver_type resolver(udp_server);
        auto destiny = *resolver.resolve(query_type(type::ip::udp::v4(), m_next_name, "62000"));

        std::cout << "xxxxxx " << m_hostname << std::endl << std::flush;
        socket.send_to(type::network::buffer(message), destiny, 0, error);

        error.clear();
    }

    type::udp::endpoint_type predecessor;
    
    resolver_type resolver(udp_server);
    auto next_address = *resolver.resolve(query_type(type::ip::udp::v4(), m_next_name, "62000"));

    while (true)
    {
        //! Wait token.
        std::cout << "yyyyyy " << m_hostname << std::endl << std::flush;
        socket.receive_from(type::network::buffer(message), predecessor, 0, error);

        //! Releases thread main.
        m_critical_mutex.unlock();
        m_standby_mutex.lock();

        m_critical_mutex.lock();
        m_standby_mutex.unlock();

        //! Sends token.
        strcpy(message, m_hostname.c_str());
        std::cout << "zzzzzz " << m_hostname << std::endl << std::flush;
        
        error.clear();
        socket.send_to(type::network::buffer(message), next_address, 0, error);
    }
}

} // namespace lock