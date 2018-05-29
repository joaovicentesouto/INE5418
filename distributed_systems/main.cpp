
#include <iostream>
#include <cstdlib>

#include "locks/LocksmithCS.hpp"
#include "locks/LocksmithMulticast.hpp"
#include "locks/LocksmithToken.hpp"

int main(int argc, char *argv[])
{
    if(const char* env_p = std::getenv("HOME"))
        std::cout << "Your HOME is: " << env_p << '\n';

    return 0;
}
