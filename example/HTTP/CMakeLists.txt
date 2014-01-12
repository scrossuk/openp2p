add_executable(httpClient
	mainClient.cpp
	)

target_link_libraries(httpClient
	openp2p-tcp
	openp2p-ip
	openp2p-stream
	openp2p-event
	openp2p
	boost_thread
	boost_system
	pthread
	)

