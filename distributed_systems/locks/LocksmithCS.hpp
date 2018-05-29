#ifndef LOCKS_CS_HPP
#define LOCKS_CS_HPP

#include "Locksmith.hpp"
#include "../types/TCPTypes.hpp"

namespace locks
{
    class LocksmithCS : public Locksmith
    {
    public:
        using address_type = type::tcp::address_type;

        // Class constructors
        LocksmithCS(const address_type &server_addr);

        LocksmithCS(const LocksmithCS&) = delete;
        LocksmithCS& operator=(const LocksmithCS&) = delete;

        LocksmithCS(LocksmithCS&&) = default;
        LocksmithCS& operator=(LocksmithCS&&) = default;

        void llock();
        void lunlock();

    private:
        const address_type m_server_addr;
    };

}   // namespace lock

#endif  // LOCKS_CS_HPP
