#ifndef LOCKS_MULTICAST_HPP
#define LOCKS_MULTICAST_HPP

#include "Locksmith.hpp"
#include <distributed_systems/types/UDPTypes.hpp>

namespace locks
{
    class LocksmithMulticast : public Locksmith
    {
    public:
        using address_type = type::udp::address_type;

        // Class constructors
        LocksmithMulticast();

        LocksmithMulticast(const LocksmithMulticast&) = delete;
        LocksmithMulticast& operator=(const LocksmithMulticast&) = delete;

        LocksmithMulticast(LocksmithMulticast&&) = default;
        LocksmithMulticast& operator=(LocksmithMulticast&&) = default;

        void llock();
        void lunlock();

    // private:
    //     const address_type m_multicast_addr;
    };

}   // namespace lock

#endif  // LOCKS_MULTICAST_HPP
