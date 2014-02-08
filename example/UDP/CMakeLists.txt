add_executable(udpClient
	client.cpp
	)

add_executable(udpServer
	server.cpp
	)

target_link_libraries(udpClient
	openp2p-udp
	openp2p-ip
	openp2p-buffer
	openp2p-stream
	openp2p-event
	openp2p
	boost_thread
	boost_system
	pthread
	)

target_link_libraries(udpServer
	openp2p-udp
	openp2p-ip
	openp2p-buffer
	openp2p-stream
	openp2p-event
	openp2p
	boost_thread
	boost_system
	pthread)
