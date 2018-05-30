################################################################################
# This is the main CMakeLists file for the pratical work for INE5418.
#
# Its main goals are:
#   - Set up global variables.
#   - Configure dependencies (find_package).
#   - Add subdirectories.
#
################################################################################

################################################################################
# Set up global variables
################################################################################

# Set up minimal cmake version
cmake_minimum_required(VERSION 3.5.1)

# Set project name
project(distributed_systems)

# Set project version
set(distributed_systems_VERSION 0.1)

# Set c++14 globally
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Set distributed_systems source dir
set(distributed_systems_source_dir ${PROJECT_SOURCE_DIR})

################################################################################
# Handle raquirements
################################################################################

# Find Boost
find_package(Boost 1.58 REQUIRED)

################################################################################
# Project logic 
################################################################################

# Add subdirectories
add_subdirectory(distributed_systems)
add_subdirectory(tests)
