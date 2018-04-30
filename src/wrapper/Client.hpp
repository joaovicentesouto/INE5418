#include "../connection_types/BasicTypes.hpp"

namespace wrapper
{
    class Client
    {
    public:
        using string_type = types::string_type;

        // Class constructors
        virtual ~Client() = default;

        // Class member functions
        virtual void write(const string_type& msg) = 0;
        virtual const string_type read() = 0;

    private:
    };
}   // namespace wrapper
