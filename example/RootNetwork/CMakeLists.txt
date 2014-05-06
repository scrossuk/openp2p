add_executable(client
	client.cpp
	)

target_link_libraries(client
	openp2p-rootnetwork
	openp2p-udp
	openp2p-ip
	openp2p-crypt
	openp2p-concurrency
	openp2p-transport
	openp2p-event
	openp2p-util
	boost_thread
	boost_system
	cryptopp
	pthread
	)

