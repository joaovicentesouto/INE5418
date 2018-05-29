#!/bin/zsh

echo "TESTE......"
mkdir build
cd build
cmake ..
make
cd distributed_systems
./distributed_systems_executable