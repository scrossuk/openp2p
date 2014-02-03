include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(FUSE)

add_executable(folderSync
	FileSystemWrapper.cpp
	HandleRef.cpp
	NodeSystem.cpp
	main.cpp
	)

target_link_libraries(folderSync
	openp2p-foldersync
	openp2p-crypt
	openp2p-stream
	openp2p-event
	openp2p
	fusewrapper
	fuse
	cryptopp
	pthread
	)

