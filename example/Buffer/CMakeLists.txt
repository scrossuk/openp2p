add_executable(bufferTest
	main.cpp
	)

target_link_libraries(bufferTest
	openp2p-transport
	openp2p-event
	boost_thread
	boost_system
	pthread
	)

