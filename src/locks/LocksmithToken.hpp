#include "Locksmith.hpp"
#include "../wrapper/UDPTypes.hpp"

namespace locks
{
    class LocksmithToken : public Locksmith
    {
    public:
        using address_type = types::address_type;

        // Class constructors
        LocksmithToken(const address_type &next_addr);

        LocksmithToken(const LocksmithToken&) = delete;
        LocksmithToken& operator=(const LocksmithToken&) = delete;

        LocksmithToken(LocksmithToken&&) = default;
        LocksmithToken& operator=(LocksmithToken&&) = default;

        void llock();
        void lunlock();

    private:
        const address_type m_next_addr;
    };

}   // namespace lock