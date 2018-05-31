#!/bin/zsh

echo "* * * Entering INE5418"
cd distr

echo "* * * Creating build folder"
mkdir build

echo "* * * Entering build folder"
cd build

echo "* * * Run CMake"
cmake ..

echo "* * * Run make"
make