#ifndef LOCKS_TOKEN_HPP
#define LOCKS_TOKEN_HPP

#include "Locksmith.hpp"
#include <distributed_systems/types/UDPTypes.hpp>

namespace locks
{
    class LocksmithToken : public Locksmith
    {
    public:
        using string_type   = type::string_type;

        using protocol_type   = type::udp::protocol_type;
        using io_service_type = type::udp::io_service_type;
        using socket_type     = type::udp::socket_type;
        using resolver_type   = type::udp::resolver_type;
        using query_type      = type::udp::query_type;
        using address_type    = type::udp::address_type;

        // Class constructors
        LocksmithToken();

        LocksmithToken(const LocksmithToken&) = delete;
        LocksmithToken& operator=(const LocksmithToken&) = delete;

        LocksmithToken(LocksmithToken&&) = default;
        LocksmithToken& operator=(LocksmithToken&&) = default;

        void llock();
        void lunlock();

    private:
        void ring_algorithm();

        const string_type m_next_name;
        const string_type m_hostname;

        std::mutex m_mutex;
    };

}   // namespace lock

#endif  // LOCKS_TOKEN_HPP
