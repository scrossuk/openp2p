add_executable(eventTest main.cpp)
target_link_libraries(eventTest openp2p-event openp2p boost_thread boost_system pthread)
