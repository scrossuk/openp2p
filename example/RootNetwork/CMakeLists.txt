add_executable(client1
	client1.cpp
	)

add_executable(client2
	client2.cpp
	)

target_link_libraries(client1
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
	)

target_link_libraries(client2
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
	)

