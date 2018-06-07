################################################################################
# This is the CMakeLists file for the:
#
#   Boost asio library tests
#
# Its main goals are:
#   - Test the library for tcp/udp communication.
#
################################################################################

################################################################################
# Test executable
################################################################################

# Add server executable
add_executable( boost_udp_server "./server.cpp" )

target_link_libraries(boost_udp_server PUBLIC
    PUBLIC boost_system
)

# Add client executable
add_executable( boost_udp_client "./client.cpp" )

target_link_libraries(boost_udp_client PUBLIC
    PUBLIC boost_system
)