add_executable(timerTest main.cpp)

target_link_libraries(timerTest
	openp2p-event
	openp2p
	boost_thread
	boost_system
	pthread
	)
