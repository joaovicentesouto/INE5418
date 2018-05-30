
#include <iostream>
#include <cstdlib>

#include "locks/LocksmithCS.hpp"
#include "locks/LocksmithMulticast.hpp"
#include "locks/LocksmithToken.hpp"

locks::Locksmith * locksmith;

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

    // const char* shared_file_path = std::getenv("SHARED_FILE");
    // const char* hibernation_time = std::getenv("HIBERNATION_TIME");

    // if(!shared_file_path || !hibernation_time)
    // {
    //     std::cout << "Could not read environment variables.\n";
    //     return 1;
    // }

    // int count = 0;
    // while (count++ < 6)
    // {
    //     locksmith->llock();
    //     std::cout << count << ": Critical region\n";
    //     locksmith->lunlock();

    //     std::cout << "Go to sleep\n";
    //     sleep(std::stoi(hibernation_time));
    //     std::cout << "Wake up\n";
    // }

    // delete shared_file_path;
    // delete hibernation_time;

    return 0;
}
