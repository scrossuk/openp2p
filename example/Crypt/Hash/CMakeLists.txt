add_executable(sha256HashExample
	sha256HashExample.cpp
	)

target_link_libraries(sha256HashExample
	openp2p-crypt
	openp2p-stream
	openp2p-event
	openp2p
	boost_thread
	boost_system
	cryptopp
	pthread
	)

