#include "LocksmithMulticast.hpp"

namespace locks
{

LocksmithMulticast::LocksmithMulticast() :
    m_containers_amount(std::stoi(std::getenv("CONTAINERS_AMOUNT"))),
    m_request_addr(type::ip::address::from_string(std::getenv("MULTCAST_REQUEST"))),
    m_exit_addr(type::ip::address::from_string(std::getenv("MULTCAST_CONFIRM")))
{
    /*
        Pensar no comportamento ...
    */
}

void LocksmithMulticast::llock()
{
    /*
        Pensar no comportamento ...
    */
}

void LocksmithMulticast::lunlock()
{
    /*
        Pensar no comportamento ...
    */
}

} // namespace lock