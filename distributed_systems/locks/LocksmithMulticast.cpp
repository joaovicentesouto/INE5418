#include "LocksmithMulticast.hpp"

namespace locks
{

Key::Key(size_t age, string_type owner) :
    m_clock(age)
{
    strcpy(m_owner, owner.c_str());
}

LocksmithMulticast::LocksmithMulticast() :
    m_id(std::stoi(std::getenv("ID"))),
    m_clock(m_id),
    m_hostname(string_type("container") + std::getenv("ID")),
    m_host_endpoint(type::ip::udp::v4(), 62000),
    m_containers_amount(std::stoi(std::getenv("CONTAINERS_AMOUNT"))),
    m_request_addr(type::ip::address::from_string(std::getenv("MULTCAST_REQUEST"))),
    m_confirm_addr(type::ip::address::from_string(std::getenv("MULTCAST_CONFIRM"))),
    m_udp_service(),
    m_socket(m_udp_service)
{
    m_socket.open(m_host_endpoint.protocol());
    m_socket.set_option(type::ip::udp::socket::reuse_address(true));
    m_socket.bind(m_host_endpoint);
    m_socket.set_option(type::ip::multicast::join_group(m_request_addr));

    type::thread_type(&LocksmithMulticast::lamport_algorithm, this).detach();
 
    sleep(1);
}

void LocksmithMulticast::llock()
{
    m_mutex.lock();
}

void LocksmithMulticast::lunlock()
{
    m_mutex.unlock();
}

void LocksmithMulticast::lamport_algorithm()
{
    m_mutex.lock(false);

    m_socket.async_receive_from(
        type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), m_requester, 
        boost::bind(&LocksmithMulticast::handle_receive_from, this, 
          type::network::placeholders::error,
          type::network::placeholders::bytes_transferred));

    m_udp_service.run();
}

void LocksmithMulticast::handle_receive_from(const type::error_type& error, size_t bytes_recvd)
{
    type::error_type ignored_error;

    if (!error && bytes_recvd > 0)
    {
        if (m_mutex.on_standyby())
        {
            m_socket.set_option(type::ip::multicast::join_group(m_confirm_addr));
            
            type::udp::endpoint_type multicast_req(m_request_addr, 62000);
            m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), multicast_req, 0, ignored_error);

            type::udp::endpoint_type m_responser;
            while (m_oks.size() < m_containers_amount-1)
            {
                m_socket.receive_from(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), m_responser, 0, ignored_error);

                if (m_key.m_type)
                    m_oks.insert(m_responser);
                else
                {
                    if (m_clock < m_key.m_clock)
                    {

                    }
                }
            }
        }
        else
        {
            m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), m_requester, 0, ignored_error);
        }
    }
    else
    {
        m_socket.async_receive_from(
            type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), m_requester, 
            boost::bind(&LocksmithMulticast::handle_receive_from, this, 
            type::network::placeholders::error,
            type::network::placeholders::bytes_transferred));
    }
}

} // namespace lock