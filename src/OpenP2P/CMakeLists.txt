add_library(openp2p STATIC
	IOService.cpp
	Signal.cpp
	String.cpp
	Thread.cpp
	Timer.cpp
	)

add_subdirectory(Buffer)

add_subdirectory(Crypt)

add_subdirectory(Event)

add_subdirectory(FolderSync)

add_subdirectory(IP)

add_subdirectory(OFTorrent)

add_subdirectory(RootNetwork)

add_subdirectory(Stream)

add_subdirectory(TCP)

add_subdirectory(UDP)

