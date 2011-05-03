add_library(openp2p STATIC BinaryStream.cpp Buffer.cpp BufferBuilder.cpp BufferIterator.cpp BufferQueue.cpp IOService.cpp Signal.cpp Thread.cpp Timeout.cpp)
add_subdirectory(RootNetwork)
add_subdirectory(TCP)
add_subdirectory(UDP)
