#include "LocksmithMulticast.hpp"

namespace locks
{

Key::Key(unsigned clock, string_type owner) : m_clock(clock)
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
    std::cout << "Initiating multicast/lamport algorithm." << std::flush;
    sleep(10);

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
        std::cout << "1. recebi msg de: " << question_key.m_owner
                  << " t:" << question_key.m_type
                  << " c:" << question_key.m_clock
                  << std::endl << std::flush;

        //! Ignore my own messages.
        if (!strcmp(m_key.m_owner, question_key.m_owner))
            continue;

        m_request_mutex.lock();

        if (m_critical_region_request)
        {
            std::cout << "__ AGORA EU QUERO\n" << std::flush;
            size_t major_clock = m_key.m_clock;

            while (m_oks.size() < m_containers_amount - 1)
            {
                //! Ignore my own messages.
                if (!strcmp(m_key.m_owner, question_key.m_owner))
                    continue;

                if (question_key.m_type)
                {
                    std::cout << "__ Quero. e é msg ok\n" << std::flush;
                    m_oks.emplace(question_key.m_owner);
                }
                else if (m_oks.find(question_key.m_owner) == m_oks.end())
                {
                    std::cout << "__ Quero. e não é ok, \n" << std::flush;
                    if (m_key < question_key)
                    {
                        std::cout << "__ tenho pref. m:" << m_key.m_clock << " q:" << question_key.m_clock << std::endl << std::flush;
                        m_oks.emplace(question_key.m_owner);
                    }
                    else
                    {
                        //! I have no priority so send Ok.
                        std::cout << "__ não tenho pref. m:" << m_key.m_clock << " q:" << question_key.m_clock << std::endl << std::flush;
                        m_key.m_type = true;
                        m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), requester, 0, error); //! ok
                    }
                }

                //! Saves the major iime received.
                major_clock = major_clock < question_key.m_clock ? question_key.m_clock : major_clock;

                std::cout << "^^OKs received\n" << std::flush;
                for (auto& addr : m_oks)
                    std::cout << addr << std::endl << std::flush;
                std::cout << std::endl << std::flush;

                if (m_oks.size() < m_containers_amount - 1)
                {
                    m_socket.receive_from(type::network::buffer(reinterpret_cast<char *>(&question_key), sizeof(Key)), requester, 0, error);
                    std::cout << "^^ 2. recebi msg de: " << question_key.m_owner
                            << " t:" << question_key.m_type
                            << " c:" << question_key.m_clock
                            << std::endl << std::flush;
                }
            }

            //! Releases thread main to enter the critical region.
            m_critical_mutex.unlock();
            m_request_mutex.lock();            
            m_critical_mutex.lock();

            //! Updates clock and sends Ok to anyone waiting.
            major_clock = major_clock + m_containers_amount;
            m_key.m_clock = major_clock + (m_containers_amount - (major_clock % m_containers_amount)) + m_id;
            m_key.m_type = true;
            std::cout << m_hostname << ": " << m_key.m_clock << std::endl << std::flush;
            std::cout << "^^ Envia ok para todo mundo.\n" << std::flush;

            m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), multicast_endpoint, 0, error);
            m_oks.clear();
        }
        else if (!question_key.m_type)
        {
            std::cout << "Não quero e manda ok: " << question_key.m_owner << std::endl << std::flush;
            //! As I have no interest and the message is a request, responds with Ok.
            m_key.m_type = true;
            m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), requester, 0, error);
        }
        else
        {
            std::cout << "* Não quero é de ok: " << question_key.m_owner << std::endl << std::flush;
        }

        m_request_mutex.unlock();
    }
}

} // namespace locks
