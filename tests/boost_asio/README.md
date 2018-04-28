# Description

Echo server using boost asio.

# Compile with:
* client: g++ -o client client.cpp -lboost_system
* server: g++ -o server server.cpp -std=c++14 -lboost_system -pthread