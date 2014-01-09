add_executable(ecdsaTest main.cpp)
target_link_libraries(ecdsaTest boost_thread boost_system cryptopp openp2p openp2p-crypt)
