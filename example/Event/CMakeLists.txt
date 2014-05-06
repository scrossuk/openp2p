add_executable(eventTest
	main.cpp
	)

target_link_libraries(eventTest
	openp2p-concurrency
	openp2p-event
	boost_thread
	boost_system
	pthread
	)
