#include "LocksmithToken.hpp"

namespace locks
{

LocksmithToken::LocksmithToken() :
    m_next_name(string_type("container") + std::to_string((std::stoi(std::getenv("ID"))+1) % std::stoi(std::getenv("CONTAINERS_AMOUNT")))),
    m_hostname(string_type("container") + std::getenv("ID"))
{
    type::thread_type(&LocksmithToken::ring_algorithm, this).detach();
}

void LocksmithToken::lock()
{
    m_standby_mutex.lock();
    m_critical_mutex.lock();
    m_standby_mutex.unlock();
}

void LocksmithToken::unlock()
{
    m_critical_mutex.unlock();
}

void LocksmithToken::ring_algorithm()
{
    m_critical_mutex.lock();
    char message[100];

    type::network::io_service udp_server;
    type::udp::socket_type sock(udp_server, protocol_type::endpoint(type::ip::udp::v4(), 62000));

    if (m_hostname == "container2")
    {
        resolver_type resolver(udp_server);
        auto destiny = *resolver.resolve(query_type(type::ip::udp::v4(), m_next_name, "62000"));

        type::error_type ignored_error;
        sock.send_to(type::network::buffer(message), destiny, 0, ignored_error);
    }

    while (true)
    {
        type::udp::endpoint_type predecessor;
        type::error_type error;

        sock.receive_from(type::network::buffer(message), predecessor, 0, error);

        m_critical_mutex.unlock();

        m_standby_mutex.lock();
        m_critical_mutex.lock();
        m_standby_mutex.unlock();

        strcpy(message, m_hostname.c_str());

        resolver_type resolver(udp_server);
        auto destiny = *resolver.resolve(query_type(type::ip::udp::v4(), m_next_name, "62000"));

        type::error_type ignored_error;
        sock.send_to(type::network::buffer(message), destiny, 0, ignored_error);
    }
}

} // namespace lock