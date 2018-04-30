
namespace connection
{
    class Server
    {
    public:

        // Class constructors
        virtual ~Server() = default;

        // Class member functions
        virtual void run() = 0;

    };
}   // namespace connection
