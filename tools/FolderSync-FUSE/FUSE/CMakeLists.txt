add_definitions(-D_FILE_OFFSET_BITS=64)

add_library(fusewrapper
	FileSystem.cpp
	Path.cpp
	)

