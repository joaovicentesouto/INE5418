################################################################################
# This is the CMakeLists file for the:
#
#   namespace distributed_systems::locks
#
# Its main goals are:
#   - Fetch library files.
#   - Add target.
#       `- Set target_include_path.
#       `- Set target_link_libraries.
#       `- Set target_compiler_options.
#   - Define installation parameters.
#       `- Install targets.
#       `- Install headers.
#
################################################################################

################################################################################
# Set variables
################################################################################

# Set the include path for installed target
set(distributed_systems_locks_install_include_dir 
    ${distributed_systems_install_include_dir}/distributed_systems/locks
)

################################################################################
# Fetch files
################################################################################

# Fetch .ccp files for library creation
file(GLOB distributed_systems_locks_source
    "*.cpp"
)

# Fetch .h files for library creation
file(GLOB distributed_systems_locks_headers
    "*.h"
)

################################################################################
# Library target
################################################################################

# Add library target
add_library(distributed_systems_locks SHARED ${distributed_systems_locks_source})

# Set shared library version, this will make cmake create a link
set_target_properties(distributed_systems_locks PROPERTIES
    VERSION ${distributed_systems_VERSION}
    SOVERSION ${distributed_systems_VERSION}
)

# Tell cmake target's dependencies
target_link_libraries(distributed_systems_locks
    PUBLIC distributed_systems_types
)

# Tell cmake the path to look for include files for this target
# This is transitive, so any targets linked with this one will
# search for headers in the respective paths
target_include_directories(distributed_systems_locks PUBLIC
    $<BUILD_INTERFACE:${distributed_systems_source_dir}>
    $<INSTALL_INTERFACE:include>
)

# Add library target
#add_library(distributed_systems_locks_static STATIC ${distributed_systems_locks_source})

# Tell cmake target's dependencies
#target_link_libraries(distributed_systems_locks_static
#)

# Tell cmake the path to look for include files for this target
# This is transitive, so any targets linked with this one will
# search for headers in the respective paths
#target_include_directories(distributed_systems_locks_static PUBLIC
#    $<BUILD_INTERFACE:${distributed_systems_source_dir}>
#    $<INSTALL_INTERFACE:include>
#)
