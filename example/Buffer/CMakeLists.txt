add_executable(bufferTest
	main.cpp
	)

target_link_libraries(bufferTest
	openp2p-buffer
	openp2p-stream
	openp2p-event
	openp2p
	boost_thread
	boost_system
	pthread
	)

