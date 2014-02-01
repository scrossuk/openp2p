# Enable performance information.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

add_definitions(-D_FILE_OFFSET_BITS=64)

add_executable(folderSync
	FileSystem.cpp
	main.cpp
	Path.cpp
	)

target_link_libraries(folderSync
	openp2p-foldersync
	openp2p-crypt
	openp2p-stream
	openp2p-event
	openp2p
	fuse
	cryptopp
	pthread
	)

