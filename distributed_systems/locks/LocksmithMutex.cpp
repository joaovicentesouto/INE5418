#include "LocksmithMutex.hpp"

namespace locks
{

bool LocksmithMutex::swap()
{
    type::mutex_type m_helper_mutex;
    type::unique_lock_type lck(m_helper_mutex);
    m_on_standby = !m_on_standby;
}

bool LocksmithMutex::on_standby()
{
    type::mutex_type m_helper_mutex;
    type::unique_lock_type lck(m_helper_mutex);
    return m_on_standby;
}

} // namespace locks
