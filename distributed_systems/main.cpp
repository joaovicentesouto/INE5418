
#include <iostream>
#include <fstream>
#include <cstdlib>

#include "locks/LocksmithCS.hpp"
#include "locks/LocksmithMulticast.hpp"
#include "locks/LocksmithToken.hpp"

struct Field
{
    char m_container_name[100] = "container-";
    size_t m_count{0u};
};

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        std::cout << "Please inform the desired synchronization algorithm:\n"
                  << "1: Client/Server\n"
                  << "2: Token ring\n"
                  << "3: Multicast/Lamport\n" << std::flush;
        return 1;
    }

    locks::Locksmith *locksmith;

    switch (std::stoi(argv[1]))
    {
    case 1:
        locksmith = new locks::LocksmithCS();
        break;
    case 2:
        locksmith = new locks::LocksmithToken();
        break;
    case 3:
        locksmith = new locks::LocksmithMulticast();
        break;
    default:
        std::cout << "Invalid option.\n" << std::flush;
        return 1;
    }

    /* ----- Environment variables ----- */
    type::string_type shared_file_path{std::getenv("SHARED_FILE")};
    type::string_type hibernation_time{std::getenv("HIBERNATION_TIME")};
    type::string_type hostname = type::string_type("container") + std::getenv("ID");

    if (shared_file_path.empty() || hibernation_time.empty())
    {
        std::cout << "Could not read environment variables.\n" << std::flush;
        return 1;
    }

    Field field;

    int count = 0;
    while (count++ < 100)
    {
        locksmith->lock();

        /* ----- READ MODE ----- */

        std::ifstream in(shared_file_path, std::ifstream::binary);

        if (in.is_open())
        {
            in.seekg(0, in.beg);
            in.read(reinterpret_cast<char *>(&field), sizeof(Field));
        }

        in.close();
        in.clear();

        std::cout << std::endl << std::endl
                  << "Event: " << field.m_count
                  << " | Predecessor: " << field.m_container_name
                  << std::endl << std::flush;

        /* ----- WRITE MODE ----- */

        std::ofstream out(shared_file_path, std::ios::binary | std::ios::trunc);
        
        strcpy(field.m_container_name, hostname.c_str());
        field.m_count++;

        out.write(reinterpret_cast<char *>(&field), sizeof(Field));

        std::cout << "Event: " << field.m_count
                  << " | Current    : " << field.m_container_name
                  << std::endl << std::endl << std::flush;

        out.close();
        out.clear();

        locksmith->unlock();

        for (auto i = 1; i <= std::stoi(hibernation_time); i++, sleep(1))
            std::cout << ". " << std::flush;

        std::cout << std::endl;
    }

    return 0;
}
