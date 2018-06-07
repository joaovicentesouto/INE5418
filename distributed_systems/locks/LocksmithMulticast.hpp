#ifndef LOCKS_LOCKSMITH_MULTICAST_HPP
#define LOCKS_LOCKSMITH_MULTICAST_HPP

#include "Locksmith.hpp"
#include <distributed_systems/locks/Mutex.hpp>
#include <distributed_systems/types/UDPTypes.hpp>

namespace locks
{
    class LocksmithMulticast : public Locksmith
    {
    public:
        using address_type = type::ip::address;

        // Class constructors
        LocksmithMulticast();

        LocksmithMulticast(const LocksmithMulticast&) = delete;
        LocksmithMulticast& operator=(const LocksmithMulticast&) = delete;

        LocksmithMulticast(LocksmithMulticast&&) = default;
        LocksmithMulticast& operator=(LocksmithMulticast&&) = default;

        void llock();
        void lunlock();

    private:
        const address_type m_request_addr, m_exit_addr;
        const unsigned     m_containers_amount;
        LocksmithMutex     m_mutex;
    };

}   // namespace lock

#endif  // LOCKS_LOCKSMITH_MULTICAST_HPP
