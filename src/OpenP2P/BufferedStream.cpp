#include <algorithm>
#include <OpenP2P/BufferedStream.hpp>
#include <OpenP2P/Future.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	BufferedStream::BufferedStream(IStream& stream, std::size_t bufferSize)
		: stream_(stream), data_(new uint8_t[bufferSize]), bufferSize_(bufferSize), readPos_(0), writePos_(0){ }
		
	Future<std::size_t> BufferedStream::readSome(std::size_t requestedSize){
		if(requestedSize <= size_()) return Future<std::size_t>(requestedSize);
		std::size_t availableSpace = bufferSize_ - writePos_;
		std::size_t maxReadSize = std::min(availableSpace, requestedSize - size_());
		
		Future<std::size_t> read = stream_.readSome(data_.get() + writePos_, maxReadSize);
		return read.compose<std::size_t>(boost::bind(&BufferedStream::onRead, this, requestedSize, _1));
	}

	void BufferedStream::consume(std::size_t requestedSize){
		Lock lock(mutex_);
		readPos_ += std::min(requestedSize, size_());
		if(readPos_ == bufferSize_) readPos_ = writePos_ = 0;
	}
	
	std::size_t BufferedStream::onRead(std::size_t requestedSize, std::size_t readSize){
		Lock lock(mutex_);
		writePos_ += readSize;
		return std::min(requestedSize, size_());
	}

}

