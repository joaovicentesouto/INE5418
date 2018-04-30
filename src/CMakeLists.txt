################################################################################
# This is the CMakeLists file for the distr_systens directory.
#
################################################################################

# Add all subdirectories
# Each subdirectory target must be exported
add_subdirectory(locks)
add_subdirectory(types)
add_subdirectory(wrapper)

# Fetch test files recursevely
file(GLOB_RECURSE distr_systens_source
    "*.cpp"
    "*.h"
)

# Add test target
add_executable(distr_systens_executable ${distr_systens_source})

# Link target dependencies _ é preciso criar libs para eles ???
#target_link_libraries(distr_systens_executable
#    PRIVATE distr_systens_locks_static
#    PRIVATE distr_systens_types_static
#    PRIVATE distr_systens_wrapper_static
#)
