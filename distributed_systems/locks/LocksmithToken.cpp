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

    type::udp::socket_type socket(udp_server, protocol_type::endpoint(type::ip::udp::v4(), 62000));

    resolver_type resolver(udp_server);
    type::udp::endpoint_type predecessor;
    type::udp::endpoint_type next_address = *resolver.resolve(query_type(type::ip::udp::v4(), m_next_name, "62000"));

    //! Last container starts sending token.
    if (m_next_name == "container0")
        socket.send_to(type::network::buffer(message), next_address, 0, error);

    while (true)
    {
        //! Wait token.
        socket.receive_from(type::network::buffer(message), predecessor, 0, error);

        //! Releases thread main.
        m_critical_mutex.unlock();
        m_standby_mutex.lock();

        m_critical_mutex.lock();
        m_standby_mutex.unlock();

        //! Sends token.
        strcpy(message, m_hostname.c_str());
        socket.send_to(type::network::buffer(message), next_address, 0, error);
    }
}

} // namespace lock