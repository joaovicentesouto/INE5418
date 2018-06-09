#ifndef LOCKS_LOCKSMITH_MULTICAST_HPP
#define LOCKS_LOCKSMITH_MULTICAST_HPP

#include <vector>
#include "Locksmith.hpp"
#include <distributed_systems/types/UDPTypes.hpp>

namespace locks
{
    struct Key
    {
        using string_type = type::string_type;

        Key() = default;
        Key(size_t clock, string_type owner);

        bool operator<(const Key& another_key);

        size_t m_clock{0};
        char   m_owner[100] = "No owner";
        bool   m_request{true}; //! false: ok, true: request
    };

    class LocksmithMulticast : public Locksmith
    {
    public:
        using address_type    = type::ip::address;
        using string_type     = type::string_type;

        using protocol_type   = type::udp::protocol_type;
        using io_service_type = type::udp::io_service_type;
        using socket_type     = type::udp::socket_type;
        using resolver_type   = type::udp::resolver_type;
        using query_type      = type::udp::query_type;
        using endpoint_type   = type::udp::protocol_type::endpoint;

        // Class constructors
        LocksmithMulticast();

        LocksmithMulticast(const LocksmithMulticast&) = delete;
        LocksmithMulticast& operator=(const LocksmithMulticast&) = delete;

        LocksmithMulticast(LocksmithMulticast&&) = default;
        LocksmithMulticast& operator=(LocksmithMulticast&&) = default;

        void lock();
        void unlock();

    private:
        void lamport_algorithm();
        void check_deadlock();
        void handle_receive_from(const type::error_type& error, size_t bytes_recvd);

        type::mutex_type    m_critical_mutex;
        type::mutex_type    m_request_mutex;
        const size_t        m_id, m_containers_amount;
        const string_type   m_hostname;
        const endpoint_type m_host_endpoint;
        const address_type  m_request_addr, m_confirm_addr;

        type::network::io_service     m_udp_service;
        type::udp::socket_type        m_socket;

        Key m_key;
        bool m_critical_region_request{false}, m_deadline{false};
    };

}   // namespace lock

#endif  // LOCKS_LOCKSMITH_MULTICAST_HPP
