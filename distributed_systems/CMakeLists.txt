################################################################################
# This is the CMakeLists file for the distributed_systems directory.
#
################################################################################

# Add all subdirectories
# Each subdirectory target must be exported
add_subdirectory(locks)
add_subdirectory(types)

# Add test target
add_executable(distributed_systems_executable "main.cpp")

target_link_libraries(distributed_systems_executable
    PRIVATE distributed_systems_locks
)
