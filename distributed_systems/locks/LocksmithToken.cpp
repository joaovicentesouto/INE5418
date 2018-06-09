#include "LocksmithToken.hpp"

namespace locks
{

LocksmithToken::LocksmithToken() :
    m_next_name(string_type("container") + std::to_string((std::stoi(std::getenv("ID"))+1) % std::stoi(std::getenv("CONTAINERS_AMOUNT")))),
    m_hostname(string_type("container") + std::getenv("ID"))
{
    std::cout << "Initiating token ring algorithm." << std::flush;

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
    type::udp::socket_type socket(udp_server, protocol_type::endpoint(type::ip::udp::v4(), 62000));

    //! The last container starts sending the token.
    if (m_next_name == "container0")
    {
        resolver_type resolver(udp_server);
        auto destiny = *resolver.resolve(query_type(type::ip::udp::v4(), m_next_name, "62000"));

        type::error_type ignored_error;
        socket.send_to(type::network::buffer(message), destiny, 0, ignored_error);
    }

    type::udp::endpoint_type predecessor;

    while (true)
    {
        //! Wait token.
        type::error_type error;
        socket.receive_from(type::network::buffer(message), predecessor, 0, error);

        //! Releases thread main.
        m_critical_mutex.unlock();

        m_standby_mutex.lock();
        m_critical_mutex.lock();
        m_standby_mutex.unlock();

        strcpy(message, m_hostname.c_str());

        resolver_type resolver(udp_server);
        auto destiny = *resolver.resolve(query_type(type::ip::udp::v4(), m_next_name, "62000"));

        //! Send token.
        type::error_type ignored_error;
        socket.send_to(type::network::buffer(message), destiny, 0, ignored_error);
    }
}

} // namespace lock