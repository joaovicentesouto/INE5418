#include "LocksmithMulticast.hpp"

namespace locks
{

Key::Key(unsigned age, string_type owner) : m_clock(age)
{
    strcpy(m_owner, owner.c_str());
}

bool Key::operator<(const Key &another_key)
{
    return m_clock < another_key.m_clock;
}

LocksmithMulticast::LocksmithMulticast() :
    m_id(std::stoi(std::getenv("ID"))),
    m_hostname(string_type("container") + std::getenv("ID")),
    m_key(m_id, m_hostname),
    m_host_endpoint(type::ip::udp::v4(), 62000),
    m_containers_amount(std::stoi(std::getenv("CONTAINERS_AMOUNT"))),
    m_request_addr(type::ip::address::from_string(std::getenv("MULTCAST_REQUEST"))),
    m_confirm_addr(type::ip::address::from_string(std::getenv("MULTCAST_CONFIRM"))),
    m_udp_service(),
    m_socket(m_udp_service)
{
    //! Configures socket.
    m_socket.open(m_host_endpoint.protocol());
    m_socket.set_option(type::ip::udp::socket::reuse_address(true));
    m_socket.bind(m_host_endpoint);
    m_socket.set_option(type::ip::multicast::join_group(m_request_addr));

    type::thread_type(&LocksmithMulticast::lamport_algorithm, this).detach();
}

void LocksmithMulticast::lock()
{
    m_request_mutex.lock();

    //! Informs interest in entering the critical region.
    m_critical_region_request = true;
    m_key.m_type = false;

    type::error_type ignored_error;
    type::udp::endpoint_type multicast_endpoint(m_request_addr, 62000);

    //! Sends multicast requesting permission.
    m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), multicast_endpoint, 0, ignored_error);

    m_request_mutex.unlock();

    //! Waits for thread to receive N-1 acknowledgments.
    m_critical_mutex.lock();
}

void LocksmithMulticast::unlock()
{
    //! Coming out of the critical region.
    m_critical_region_request = false;

    m_request_mutex.unlock();
    m_critical_mutex.unlock();
}

void LocksmithMulticast::lamport_algorithm()
{
    m_critical_mutex.lock();

    Key question_key;
    type::error_type error;
    type::udp::endpoint_type requester;
    type::udp::endpoint_type multicast_endpoint(m_request_addr, 62000);

    while (true)
    {
        //! Waiting for a message.
        m_socket.receive_from(type::network::buffer(reinterpret_cast<char *>(&question_key), sizeof(Key)), requester, 0, error);

        //! Ignore my own messages.
        if (!strcmp(m_key.m_owner, question_key.m_owner))
            continue;

        m_request_mutex.lock();

        if (m_critical_region_request)
        {
            size_t major_clock = m_key.m_clock;

            while (m_oks.size() < m_containers_amount - 1)
            {
                //! Ignore my own messages.
                if (!strcmp(m_key.m_owner, question_key.m_owner))
                    continue;

                if (question_key.m_type)
                {
                    m_oks.emplace(question_key.m_owner);
                }
                else if (m_oks.find(question_key.m_owner) == m_oks.end())
                {
                    if (m_key < question_key)
                        m_oks.emplace(question_key.m_owner);
                    else
                    {
                        //! I have no priority so send Ok.
                        m_key.m_type = true;
                        m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), requester, 0, error); //! ok
                    }
                }

                //! Saves the major iime received.
                major_clock = major_clock < question_key.m_clock ? question_key.m_clock : major_clock;

                if (m_oks.size() < m_containers_amount - 1)
                    m_socket.receive_from(type::network::buffer(reinterpret_cast<char *>(&question_key), sizeof(Key)), requester, 0, error);
            }

            //! Releases thread main to enter the critical region.
            m_critical_mutex.unlock();
            m_request_mutex.lock();            
            m_critical_mutex.lock();

            //! Updates clock and sends Ok to anyone waiting.
            major_clock = major_clock + m_containers_amount;
            m_key.m_clock = major_clock + (m_containers_amount - (major_clock % m_containers_amount));
            m_key.m_type = true;

            m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), multicast_endpoint, 0, error);
            m_oks.clear();
        }
        else if (!question_key.m_type)
        {
            //! As I have no interest and the message is a request, responds with Ok.
            m_key.m_type = true;
            m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), requester, 0, error);
        }

        m_request_mutex.unlock();
    }
}

} // namespace locks
