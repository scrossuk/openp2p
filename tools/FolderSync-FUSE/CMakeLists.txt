add_definitions(-D_FILE_OFFSET_BITS=64)

add_executable(folderSync
	FileSystem.cpp
	main.cpp
	Path.cpp
	)

target_link_libraries(folderSync
	fuse
	)

