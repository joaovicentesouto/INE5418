
#include <iostream>
#include <fstream>
#include <cstdlib>

#include "locks/LocksmithCS.hpp"
#include "locks/LocksmithMulticast.hpp"
#include "locks/LocksmithToken.hpp"

locks::Locksmith *locksmith;

struct Field
{
    type::string_type m_container_name{"No owner"};
    size_t m_count{0u};
};

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        std::cout << "Please inform the desired synchronization algorithm:\n"
                  << "1: Client/server\n"
                  << "2: Lamport/Multicast\n"
                  << "3: Queue\n";
        return 1;
    }

    switch (std::stoi(argv[1]))
    {
    case 1:
        locksmith = new locks::LocksmithCS();
        break;
    case 2:
        locksmith = new locks::LocksmithMulticast();
        break;
    case 3:
        locksmith = new locks::LocksmithToken();
        break;
    default:
        std::cout << "Invalid option.\n";
        return 1;
    }

    /* ----- Environment variables ----- */
    const char *shared_file_path = std::getenv("SHARED_FILE");
    const char *hibernation_time = std::getenv("HIBERNATION_TIME");
    auto hostname = type::ip::host_name();

    if (!shared_file_path || !hibernation_time)
    {
        std::cout << "Could not read environment variables.\n";
        return 1;
    }

    int count = 0;
    while (count++ < 6)
    {
        locksmith->llock();

        /* ----- READ MODE ----- */

        Field field;
        std::ifstream in(shared_file_path, std::ifstream::binary);

        if (in.is_open())
        {
            in.seekg(0, in.beg);
            in.read(reinterpret_cast<char *>(&field), sizeof(Field));

            in.close();
        }

        /* ----- WRITE MODE ----- */

        std::ofstream out(shared_file_path, std::ios::binary | std::ios::trunc);

        std::cout << "Event: " << field.m_count
                  << " | Predecessor: " << field.m_container_name << std::endl;

        field.m_container_name = hostname;
        field.m_count++;

        out.write(reinterpret_cast<char *>(&field), sizeof(Field));

        std::cout << "Event: " << field.m_count
                  << " | Current    : " << field.m_container_name << std::endl << std::endl;

        out.close();

        locksmith->lunlock();

        for (auto i = 1; i <= std::stoi(hibernation_time); i++)
        {
            sleep(1);
            std::cout << ". " << std::flush;
        }

        std::cout << std::endl;
    }

    return 0;
}
