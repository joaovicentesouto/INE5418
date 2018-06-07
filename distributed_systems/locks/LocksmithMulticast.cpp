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

LocksmithMulticast::LocksmithMulticast() : m_id(std::stoi(std::getenv("ID"))),
                                           m_hostname(string_type("container") + std::getenv("ID")),
                                           m_key(m_id, m_hostname),
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

    // m_socket.set_option(type::ip::multicast::join_group(m_confirm_addr));

    type::thread_type(&LocksmithMulticast::lamport_algorithm, this).detach();

    sleep(10);
}

void LocksmithMulticast::llock()
{
    // // std::cout << "Preparando para dar lock\n" << std::flush;
    m_request_mutex.lock();

    m_critical_region_request = true;

    type::error_type ignored_error;

    // // std::cout << "* entre no confirm\n" << std::flush;
//    m_socket.set_option(type::ip::multicast::join_group(m_confirm_addr));

    m_key.m_type = false;
    type::udp::endpoint_type multicast_req(m_request_addr, 62000);

    // // std::cout << "Preparando para enviar multicast req\n" << std::flush;
    m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), multicast_req, 0, ignored_error);
    // // std::cout << "Mandou\n" << std::flush;

    m_request_mutex.unlock();

    // // std::cout << "Para no critical lock\n" << std::flush;
    m_critical_mutex.lock();
    // // std::cout << "vou para a regiao critica\n" << std::flush;
}

void LocksmithMulticast::lunlock()
{
    // m_request_mutex.lock();
    m_critical_region_request = false;
    m_request_mutex.unlock();

    m_critical_mutex.unlock();
}

void LocksmithMulticast::lamport_algorithm()
{
     m_critical_mutex.lock();
    // // std::cout << "* Thread criada\n" << std::flush;
    type::error_type error;
    type::udp::endpoint_type requester;
    Key question_key;
    // type::udp::endpoint_type multicast_con(m_confirm_addr, 62000);
    type::udp::endpoint_type multicast_req(m_request_addr, 62000);

    while (true)
    {

        // std::cout << "* inicio loop\n" << std::flush;
        m_socket.receive_from(type::network::buffer(reinterpret_cast<char *>(&question_key), sizeof(Key)), requester, 0, error);


        if (!strcmp(m_key.m_owner, question_key.m_owner))
            continue;

        if (error)
            throw std::out_of_range("Erro ao receber a primeira mensagem.");

        m_request_mutex.lock();

        // std::cout << "* quero regiao critica: " << m_critical_region_request << std::endl << std::flush;
        if (m_critical_region_request)
        {
            size_t major_clock = m_key.m_clock;

            while (m_oks.size() < m_containers_amount - 1)
            {

                if (!strcmp(m_key.m_owner, question_key.m_owner))
                    continue;

                // std::cout << "* recebe msg de " << question_key.m_owner << " com : " << question_key.m_clock << " x " << question_key.m_type << "\n" << std::flush;

                if (question_key.m_type)
                {
                    // std::cout << "* é um ok : size moks: " << m_oks.size() << std::endl << std::flush;
                    m_oks.emplace(question_key.m_owner);
                }
                else
                {
                    // // std::cout << "* eh request R: " << question_key.m_clock << "\n M: " << m_key.m_clock << std::endl << std::flush;

                    if (m_oks.find(question_key.m_owner) == m_oks.end())
                    {
                        if (m_key < question_key)
                        {
                            // // std::cout << "* tenho prioridade\n" << std::flush;
                            m_oks.emplace(question_key.m_owner);
                        }
                        else
                        {
                            // std::cout << "* nao tenho prio, manda ok para \n" << question_key.m_owner << std::flush;
                            m_key.m_type = true;
                            m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), requester, 0, error); //! ok
                        }
                    }
                }

                major_clock = major_clock < question_key.m_clock ? question_key.m_clock : major_clock;

                // // std::cout << "* Espera outra\n" << std::flush;
                question_key = Key();
                if (m_oks.size() < m_containers_amount - 1)
                    m_socket.receive_from(type::network::buffer(reinterpret_cast<char *>(&question_key), sizeof(Key)), requester, 0, error);
                else
                    continue;
            }

            // Send ok
            // std::cout << "* confirm: " << m_oks.size() << "\n" << std::flush;
            m_critical_mutex.unlock();
            m_request_mutex.lock();
            
            // std::cout << "* confirm: " << m_oks.size() << "\n" << std::flush;
            m_critical_mutex.lock();
            m_request_mutex.unlock();
            // std::cout << "* confirm: " << m_oks.size() << "\n" << std::flush;

            major_clock = major_clock + m_containers_amount;
            m_key.m_clock = major_clock + (m_containers_amount - (major_clock % m_containers_amount));
            m_key.m_type = true;

            // Send ok
            // // std::cout << "* envia confirm\n" << std::flush;
            m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), multicast_req, 0, error);
            m_oks.clear();
        }
        else
        {
            m_request_mutex.unlock();
            // Modify ok
            if (!question_key.m_type)
            {
                // std::cout << "* nao quero regiao . mandar ok: " << question_key.m_owner << "\n" << std::flush;
                m_key.m_type = true;
                m_socket.send_to(type::network::buffer(reinterpret_cast<char *>(&m_key), sizeof(m_key)), requester, 0, error);
            }
        }
    }
}

} // namespace locks
