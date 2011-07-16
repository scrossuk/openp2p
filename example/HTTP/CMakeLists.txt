add_executable(httpClient mainClient.cpp)
target_link_libraries(httpClient boost_thread-mt boost_system-mt openp2p openp2p-tcp)
