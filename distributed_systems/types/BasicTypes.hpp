#ifndef BASIC_TYPES_HPP
#define BASIC_TYPES_HPP

#include <string>
#include <boost/asio.hpp>

namespace type
{
    // Namespace member types
    using string_type = std::string;

    namespace asio    = boost::asio;
    namespace ip      = asio::ip;
}   // namespace types

#endif  // BASIC_TYPES_HPP