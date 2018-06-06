#include "LocksmithToken.hpp"

namespace locks
{

LocksmithToken::LocksmithToken() :
    m_next_name(string_type("container") + std::to_string((std::stoi(std::getenv("ID"))+1) % std::stoi(std::getenv("CONTAINERS_AMOUNT")))),
    m_hostname(string_type("container") + std::getenv("ID"))
{
    std::cout << "Create...\n";
    std::thread(&LocksmithToken::ring_algorithm, this).detach();
    sleep(1);
}

void LocksmithToken::llock()
{
    std::cout << "******** Main lock\n";
    m_mutex.lock();
    std::cout << "******** Main pegou lock\n";
}

void LocksmithToken::lunlock()
{
    std::cout << "******** Main unlock\n";
    m_mutex.unlock();
    std::cout << "******** Main saiu\n";
}

void LocksmithToken::ring_algorithm()
{
    m_mutex.lock();
    char message[100];

    type::network::io_service udp_server;

    type::udp::socket_type sock(udp_server, protocol_type::endpoint(type::ip::udp::v4(), 62000));

    if (m_hostname == "container2")
    {
        resolver_type resolver(udp_server);

        auto destiny = *resolver.resolve(query_type(type::ip::udp::v4(), m_next_name, "62000"));

        boost::system::error_code ignored_error;

        sock.send_to(type::network::buffer(message), destiny, 0, ignored_error);
    }

    while (true)
    {
        type::udp::address_type predecessor;
        boost::system::error_code error;

        sock.receive_from(type::network::buffer(message), predecessor, 0, error);

        m_mutex.unlock();
        m_mutex.lock();

        strcpy(message, m_hostname.c_str());

        resolver_type resolver(udp_server);

        auto destiny = *resolver.resolve(query_type(type::ip::udp::v4(), m_next_name, "62000"));


        boost::system::error_code ignored_error;

        sock.send_to(type::network::buffer(message), destiny, 0, ignored_error);
    }
}

} // namespace lock