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
add_executable( boost_asio_server "./server.cpp" )

target_link_libraries(boost_asio_server PUBLIC
    PRIVATE boost_system
)

# Add client executable
add_executable( boost_asio_client "./client.cpp" )

target_link_libraries(boost_asio_client PUBLIC
    PRIVATE boost_system
)