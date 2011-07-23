add_executable(oftorrent_create create.cpp)
target_link_libraries(oftorrent_create boost_system-mt boost_thread-mt openp2p-oftorrent openp2p)
