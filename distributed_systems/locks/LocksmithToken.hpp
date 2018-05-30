#ifndef LOCKS_TOKEN_HPP
#define LOCKS_TOKEN_HPP

#include "Locksmith.hpp"
#include <distributed_systems/types/UDPTypes.hpp>

namespace locks
{
    class LocksmithToken : public Locksmith
    {
    public:
        using address_type = type::udp::address_type;

        // Class constructors
        LocksmithToken();

        LocksmithToken(const LocksmithToken&) = delete;
        LocksmithToken& operator=(const LocksmithToken&) = delete;

        LocksmithToken(LocksmithToken&&) = default;
        LocksmithToken& operator=(LocksmithToken&&) = default;

        void llock();
        void lunlock();

    // private:
    //     const address_type m_next_addr;
    };

}   // namespace lock

#endif  // LOCKS_TOKEN_HPP
