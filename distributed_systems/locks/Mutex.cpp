#include "Mutex.hpp"

namespace locks
{

void LocksmithMutex::lock(bool main)
{
    type::unique_lock_type lck(m_helper_mutex);

    if (main)
        m_on_standby = true;

    type::mutex_type::lock();
}

void LocksmithMutex::unlock(bool main)
{
    type::unique_lock_type lck(m_helper_mutex);

    if (main)
        m_on_standby = false;

    type::mutex_type::unlock();
}

bool LocksmithMutex::on_standyby()
{
    return m_on_standby;
}

} // namespace locks
