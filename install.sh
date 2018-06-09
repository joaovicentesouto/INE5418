#!/bin/zsh

echo "* * * Entering INE5418"
cd INE5418

echo "* * * Creating build folder"
mkdir build

echo "* * * Entering build folder"
cd build

echo "* * * Run CMake"
cmake ..

echo "* * * Run make"
make -j