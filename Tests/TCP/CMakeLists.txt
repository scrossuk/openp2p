add_executable(tcpClient client.cpp)
add_executable(tcpServer server.cpp)
target_link_libraries(tcpClient boost_thread-mt boost_system-mt openp2p openp2p-tcp)
target_link_libraries(tcpServer boost_thread-mt boost_system-mt openp2p openp2p-tcp)
