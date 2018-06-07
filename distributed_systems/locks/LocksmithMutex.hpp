#ifndef LOCKS_MUTEX_HPP
#define LOCKS_MUTEX_HPP

#include <distributed_systems/types/BasicTypes.hpp>

namespace locks
{
    class LocksmithMutex : public type::mutex_type {
    public:
        LocksmithMutex() = default;

        void swap();

        bool on_standby();

    private:
        bool m_on_standby{false};
    };
}   // namespace lock

#endif // LOCKS_MUTEX_HPP
