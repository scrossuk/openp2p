add_library(openp2p STATIC
	BinaryStream.cpp
	Buffer.cpp
	BufferBuilder.cpp
	BufferedStream.cpp
	BufferIterator.cpp
	IOService.cpp
	Signal.cpp
	StringStream.cpp
	Thread.cpp
	Timer.cpp
	)

add_subdirectory(Crypt)

add_subdirectory(Event)

add_subdirectory(IP)

add_subdirectory(OFTorrent)

add_subdirectory(RootNetwork)

add_subdirectory(TCP)

add_subdirectory(UDP)

