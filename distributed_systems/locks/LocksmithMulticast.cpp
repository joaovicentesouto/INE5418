#include "LocksmithMulticast.hpp"

namespace locks
{

Key::Key(size_t age, string_type owner) : m_clock(age)
{
    strcpy(m_owner, owner.c_str());
}

bool Key::operator<(const Key& another_key)
{
    return m_clock < another_key.m_clock;
}

LocksmithMulticast::LocksmithMulticast() : m_id(std::stoi(std::getenv("ID"))),
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

    m_critical_mutex.lock();

    type::thread_type(&LocksmithMulticast::lamport_algorithm, this).detach();
}

void LocksmithMulticast::llock()
{
    m_request_mutex.lock();

    m_critical_region_request = true;

    type::error_type ignored_error;

    m_socket.set_option(type::ip::multicast::join_group(m_confirm_addr));
    type::udp::endpoint_type multicast_req(m_request_addr, 62000);
    m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), multicast_req, 0, ignored_error);

    m_request_mutex.unlock();

    m_critical_mutex.lock();
}

void LocksmithMulticast::lunlock()
{
    m_critical_mutex.unlock();
}

void LocksmithMulticast::lamport_algorithm()
{
    type::error_type error;
    type::udp::endpoint_type requester;
    Key question_key;

    while (true)
    {
        m_socket.receive_from(type::network::buffer(reinterpret_cast<char *>(&question_key), sizeof(Key)), requester, 0, error);

        if (error)
            throw std::out_of_range("Erro ao receber a primeira mensagem.");

        m_request_mutex.lock();

        if (m_critical_region_request)
        {   
            size_t major_clock = m_key.m_clock;

            do
            {
                if (question_key.m_type)
                    m_oks.insert(requester);
                else
                {
                    if (m_oks.find(requester) != m_oks.end())
                        continue;

                    if (m_key < question_key)
                    {
                        m_oks.insert(requester);
                        major_clock = major_clock < question_key.m_clock ? question_key.m_clock : major_clock;
                    }
                    else
                        m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), requester, 0, error); //! ok
                }
            } while (m_oks.size() < m_containers_amount - 1);

            m_socket.set_option(type::ip::multicast::leave_group(m_confirm_addr));
            m_socket.set_option(type::ip::multicast::leave_group(m_request_addr));

            m_critical_mutex.unlock();
            m_critical_mutex.lock();

            major_clock =  major_clock + m_containers_amount;
            m_key.m_clock = major_clock + (m_containers_amount - (major_clock % m_containers_amount));
            m_key.m_type = true;
    
            // Send ok
            type::udp::endpoint_type multicast_con(m_confirm_addr, 62000);
            m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), multicast_con, 0, error);

            m_socket.set_option(type::ip::multicast::join_group(m_request_addr));
        }
        else
        {
            // Modify ok
            if (!question_key.m_type)
            {
                m_key.m_type = true;
                m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), requester, 0, error);
            }
        }
        
        m_request_mutex.unlock();
    }
}

} // namespace locks