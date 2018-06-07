#ifndef LOCKS_MUTEX_HPP
#define LOCKS_MUTEX_HPP

#include <distributed_systems/types/BasicTypes.hpp>

namespace locks
{
    class LocksmithMutex : private type::mutex_type {
    public:
        LocksmithMutex() = default;

        void lock(bool main = true);
        void unlock(bool main = true);
        bool on_standyby();

    private:
        type::mutex_type m_helper_mutex;
        bool m_on_standby;
    };
}   // namespace lock

#endif // LOCKS_MUTEX_HPP
