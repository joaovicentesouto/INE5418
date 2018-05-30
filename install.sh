#!/bin/zsh

echo "Entering INE5418"
cd INE5418

echo "Creating build folder"
mkdir build

echo "Entering build folder"
cd build

echo "Run CMake"
cmake ..

echo "Run make"
make

# echo "Entering distributed_systems"
# cd distributed_systems

# echo "Executes executable"
# ./distributed_systems_executable