add_executable(networkGenerator
	Logger.cpp
	networkGenerator.cpp
	)

target_link_libraries(networkGenerator
	openp2p-rootnetwork
	openp2p-udp
	openp2p-ip
	openp2p-crypt
	openp2p-transport
	openp2p-concurrency
	openp2p-event
	openp2p-util
	boost_thread
	boost_system
	cryptopp
	pthread
	)

add_executable(rootNetworkClient
	Logger.cpp
	rootNetworkClient.cpp
	stringtoargcargv.cpp
	)

target_link_libraries(rootNetworkClient
	openp2p-rootnetwork
	openp2p-udp
	openp2p-ip
	openp2p-crypt
	openp2p-transport
	openp2p-concurrency
	openp2p-event
	openp2p-util
	boost_thread
	boost_system
	cryptopp
	pthread
	)

