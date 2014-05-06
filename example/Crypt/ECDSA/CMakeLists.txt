add_executable(ecdsaTest
	main.cpp
	)

target_link_libraries(ecdsaTest
	openp2p-crypt
	openp2p-transport
	openp2p-event
	boost_thread
	boost_system
	cryptopp
	pthread
	)

