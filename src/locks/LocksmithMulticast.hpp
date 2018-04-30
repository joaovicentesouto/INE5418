#include "Locksmith.hpp"
#include "../wrapper/UDPTypes.hpp"

namespace locks
{
    class LocksmithMulticast : public Locksmith
    {
    public:
        using address_type = types::address_type;

        // Class constructors
        LocksmithMulticast(const address_type &multicast_addr);

        LocksmithMulticast(const LocksmithMulticast&) = delete;
        LocksmithMulticast& operator=(const LocksmithMulticast&) = delete;

        LocksmithMulticast(LocksmithMulticast&&) = default;
        LocksmithMulticast& operator=(LocksmithMulticast&&) = default;

        void llock();
        void lunlock();

    private:
        const address_type m_multicast_addr;
    };

}   // namespace lock