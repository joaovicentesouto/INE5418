#include <string>

namespace connection
{
    class Client
    {
    public:
        using string_type = std::string;
        
        // Class constructors
        virtual ~Client() = default;

        // Class member functions
        virtual void write(const string_type& msg) = 0;
        virtual const string_type read() = 0;

    private:
    };
}   // namespace connection
