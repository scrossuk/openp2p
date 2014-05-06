add_executable(oftorrent_create
	create.cpp
	)

target_link_libraries(oftorrent_create
	openp2p-oftorrent
	openp2p-transport
	openp2p-event
	boost_system
	boost_thread
	pthread
	)

