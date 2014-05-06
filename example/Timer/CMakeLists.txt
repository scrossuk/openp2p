add_executable(timerTest main.cpp)

target_link_libraries(timerTest
	openp2p-event
	boost_thread
	boost_system
	pthread
	)
