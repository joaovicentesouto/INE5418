#ifndef BASIC_TYPES_HPP
#define BASIC_TYPES_HPP

#include <string>
#include <mutex>
#include <thread>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace type
{
    // Namespace member types
    using string_type             = std::string;
    using thread_type             = std::thread;
    using mutex_type              = std::mutex;
    using unique_lock_type        = std::unique_lock<mutex_type>;

    namespace network             = boost::asio;
    namespace ip                  = network::ip;
    using error_type              = boost::system::error_code;
}   // namespace types

#endif  // BASIC_TYPES_HPP