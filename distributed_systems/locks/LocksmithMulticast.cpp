#include "LocksmithMulticast.hpp"

namespace locks
{

Key::Key(size_t clock, string_type owner) : m_clock(clock)
{
    strcpy(m_owner, owner.c_str());
}

bool Key::operator<(const Key &another_key)
{
    if (m_clock < another_key.m_clock)
        return true;

    if (m_clock == another_key.m_clock)
        return strcmp(m_owner, another_key.m_owner) < 1;

    return false;
}

LocksmithMulticast::LocksmithMulticast() :
    m_id(std::stoi(std::getenv("ID"))),
    m_hostname(string_type("container") + std::getenv("ID")),
    m_key(m_id, m_hostname),
    m_host_endpoint(type::ip::udp::v4(), 62000),
    m_containers_amount(std::stoi(std::getenv("CONTAINERS_AMOUNT"))),
    m_request_addr(type::ip::address::from_string(std::getenv("MULTCAST_REQUEST"))),
    m_udp_service(),
    m_socket(m_udp_service)
{
    std::cout << "Initiating multicast/lamport algorithm." << std::flush;

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

    type::error_type ignored_error;
    type::udp::endpoint_type multicast_endpoint(m_request_addr, 62000);

    //! Sends multicast requesting request.
    m_key.m_request = true;
    m_key.m_clock++;
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

void LocksmithMulticast::check_deadline()
{
    sleep(0.5);

    if (m_deadline)
    {
        type::error_type ignored_error;
        type::udp::endpoint_type multicast_endpoint(m_request_addr, 62000);

        //! Sends multicast requesting request.
        m_key.m_request = true;
        m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), multicast_endpoint, 0, ignored_error);

        m_socket.cancel();
    }
}

void LocksmithMulticast::lamport_algorithm()
{
    m_critical_mutex.lock();

    Key question_key;
    type::error_type error;
    type::udp::endpoint_type requester;
    std::set<string_type> confirmed_me;
    std::set<type::udp::endpoint_type> requested_me;

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
            while (confirmed_me.size() < m_containers_amount - 1)
            {
                //! Ignore my own messages.
                if (strcmp(m_key.m_owner, question_key.m_owner))
                {
                    if (!question_key.m_request)
                        confirmed_me.emplace(question_key.m_owner);
                    else if (confirmed_me.find(question_key.m_owner) == confirmed_me.end())
                    {
                        if (m_key < question_key)
                        {
                            confirmed_me.emplace(question_key.m_owner);
                            requested_me.insert(requester);
                        }
                        else
                        {
                            //! I have no priority so send Ok.
                            m_key.m_request = false;
                            m_key.m_clock++;
                            m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), requester, 0, error); //! ok
                        }
                    }
                    else
                        requested_me.insert(requester);

                    m_key.m_clock = std::max(m_key.m_clock, question_key.m_clock) + 1;
                }

                if (confirmed_me.size() < m_containers_amount - 1)
                {
                    m_deadline = true;
                    type::thread_type(&LocksmithMulticast::check_deadline, this).detach();

                    m_socket.receive_from(type::network::buffer(reinterpret_cast<char *>(&question_key), sizeof(Key)), requester, 0, error);
                    m_deadline = false;
                }
            }

            //! Releases thread main to enter the critical region.
            m_critical_mutex.unlock();

            m_request_mutex.lock();
            m_critical_mutex.lock();

            //! Updates clock and sends Ok to anyone waiting.
            m_key.m_request = false;
            m_key.m_clock++;

            for (auto req : requested_me)
                m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), req, 0, error);

            requested_me.clear();
            confirmed_me.clear();
        }
        else if (question_key.m_request)
        {
            m_key.m_clock = std::max(m_key.m_clock, question_key.m_clock) + 1;

            //! As I have no interest and the message is a request, responds with Ok.
            m_key.m_request = false;
            m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), requester, 0, error);
        }
        else
            m_key.m_clock = std::max(m_key.m_clock, question_key.m_clock) + 1; //! Only updates the clock

        m_request_mutex.unlock();
    }
}

} // namespace locks
