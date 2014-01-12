add_executable(oftorrent_create
	create.cpp
	)

target_link_libraries(oftorrent_create
	openp2p-oftorrent
	openp2p-buffer
	openp2p-stream
	openp2p-event
	openp2p
	boost_system
	boost_thread
	pthread
	)

