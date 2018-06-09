#ifndef LOCKS_LOCKSMITH_HPP
#define LOCKS_LOCKSMITH_HPP

#include <iostream>
#include <cstdlib>
#include <utility>
#include <functional>
#include <distributed_systems/types/BasicTypes.hpp>

namespace locks
{
    class Locksmith
    {
    public:
        // Class constructors
        virtual ~Locksmith() = default;

        virtual void lock() = 0;
        virtual void unlock() = 0;
    };
}   // namespace lock

#endif // LOCKS_LOCKSMITH_HPP
