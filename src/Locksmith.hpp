
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
