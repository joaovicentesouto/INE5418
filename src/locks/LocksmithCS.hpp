#include "Locksmith.hpp"
#include "../wrapper/TCPTypes.hpp"

namespace locks
{
    class LocksmithCS : public Locksmith
    {
    public:
        using address_type = types::address_type;

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