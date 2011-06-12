add_executable(oftorrent_create create.cpp)
target_link_libraries(oftorrent_create boost_system-mt openp2p openp2p-oftorrent)
