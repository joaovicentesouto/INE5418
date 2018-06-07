#ifndef LOCKS_LOCKSMITHCS_HPP
#define LOCKS_LOCKSMITHCS_HPP

#include "Locksmith.hpp"
#include <distributed_systems/types/TCPTypes.hpp>

namespace locks
{
    class LocksmithCS : public Locksmith
    {
    public: 
        using string_type   = type::string_type;

        using protocol_type   = type::tcp::protocol_type;
        using io_service_type = type::tcp::io_service_type;
        using socket_type     = type::tcp::socket_type;
        using resolver_type   = type::tcp::resolver_type;
        using query_type      = type::tcp::query_type;
        using address_type    = type::tcp::address_type;

        // Class constructors
        LocksmithCS();

        LocksmithCS(const LocksmithCS&) = delete;
        LocksmithCS& operator=(const LocksmithCS&) = delete;

        LocksmithCS(LocksmithCS&&) = default;
        LocksmithCS& operator=(LocksmithCS&&) = default;

        void llock();
        void lunlock();

    private:
        void server_rises();

        const string_type m_hostname;
        const string_type m_server_name;

        io_service_type   m_client_service;
        socket_type       m_client;
    };

}   // namespace lock

#endif  // LOCKS_LOCKSMITHCS_HPP
