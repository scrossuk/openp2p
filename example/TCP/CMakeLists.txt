add_executable(tcpClient
	client.cpp
	)

add_executable(tcpServer
	server.cpp
	)

target_link_libraries(tcpClient
	openp2p-tcp
	openp2p-ip
	openp2p-stream
	openp2p-event
	openp2p
	boost_thread
	boost_system
	pthread
	)

target_link_libraries(tcpServer
	openp2p-tcp
	openp2p-ip
	openp2p-stream
	openp2p-event
	openp2p
	boost_thread
	boost_system
	pthread)
