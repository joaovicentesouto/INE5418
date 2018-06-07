#include "LocksmithMutex.hpp"

namespace locks
{

void LocksmithMutex::lock(bool main)
{
    m_helper_mutex.lock();

    if (main)
        m_on_standby = true;

    m_helper_mutex.unlock();

    type::mutex_type::lock();
}

void LocksmithMutex::unlock(bool main)
{
    m_helper_mutex.lock();

    if (main)
        m_on_standby = false;

    m_helper_mutex.unlock();

    type::mutex_type::unlock();
}

bool LocksmithMutex::on_standyby()
{
    type::unique_lock_type lck(m_helper_mutex);
    return m_on_standby;
}

} // namespace locks
