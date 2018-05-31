#ifndef LOCKSMITH_HPP
#define LOCKSMITH_HPP

#include <thread>
#include <iostream>
#include <cstdlib>
#include <utility>
#include <functional>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

namespace locks
{
    class Locksmith
    {
    public:
        // Class constructors
        virtual ~Locksmith() = default;

        virtual void llock() = 0;
        virtual void lunlock() = 0;
    };
}   // namespace lock

#endif // LOCKSMITH_HPP
