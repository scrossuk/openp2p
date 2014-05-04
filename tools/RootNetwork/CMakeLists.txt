add_executable(rootNetworkClient
	rootNetworkClient.cpp
	stringtoargcargv.cpp
	)

target_link_libraries(rootNetworkClient
	openp2p-rootnetwork
	openp2p-udp
	openp2p-ip
	openp2p-crypt
	openp2p-buffer
	openp2p-stream
	openp2p-event
	openp2p
	boost_thread
	boost_system
	cryptopp
	pthread
	)

