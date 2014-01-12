add_executable(ecdsaTest
	main.cpp
	)

target_link_libraries(ecdsaTest
	openp2p-crypt
	openp2p-stream
	openp2p-event
	openp2p
	boost_thread
	boost_system
	cryptopp
	pthread
	)

