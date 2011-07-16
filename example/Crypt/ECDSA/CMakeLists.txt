add_executable(ecdsaTest main.cpp)
target_link_libraries(ecdsaTest boost_thread-mt boost_system-mt cryptopp openp2p openp2p-crypt)
