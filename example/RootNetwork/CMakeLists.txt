add_executable(rootNetwork1 client1.cpp)
add_executable(rootNetwork2 client2.cpp)
target_link_libraries(rootNetwork1 boost_thread-mt boost_system-mt openp2p-rootnetwork openp2p openp2p-udp)
target_link_libraries(rootNetwork2 boost_thread-mt boost_system-mt openp2p-rootnetwork openp2p openp2p-udp)
