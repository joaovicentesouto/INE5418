################################################################################
# This is the CMakeLists file for the:
#
#   namespace distributed_systems::types
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
set(distributed_systems_types_install_include_dir 
    ${distributed_systems_install_include_dir}/distributed_systems/types
)

################################################################################
# Fetch files
################################################################################

# Fetch .h files for library creation
file(GLOB distributed_systems_types_headers
    "*.h"
)

################################################################################
# Library target
################################################################################

# Add library target
add_library(distributed_systems_types INTERFACE ${distributed_systems_types_headers})

# Tell cmake target's dependencies
target_link_libraries(distributed_systems_types
    INTERFACE boost_system
)

# Tell cmake the path to look for include files for this target
# This is transitive, so any targets linked with this one will
# search for headers in the respective paths
target_include_directories(distributed_systems_types INTERFACE
    $<BUILD_INTERFACE:${distributed_systems_source_dir}>
    $<INSTALL_INTERFACE:include>
)
