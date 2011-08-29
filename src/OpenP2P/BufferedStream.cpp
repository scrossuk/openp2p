#include <stdint.h>
#include <algorithm>
#include <cstddef>

#include <OpenP2P/BufferedStream.hpp>
#include <OpenP2P/EventHandle.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	BufferedStream::BufferedStream(IStream& stream, std::size_t bufferSize)
		: stream_(stream), data_(new uint8_t[bufferSize]),
		bufferSize_(bufferSize), readPos_(0), writePos_(0){ }
		
	EventHandle BufferedStream::readEvent(){
		// There is data available if the buffer contains data, or the
		// stream has data to read, or both.
		return EventHandle::Or(dispatcher_, stream_.readEvent());
	}
		
	std::size_t BufferedStream::read(std::size_t dataSize){
		if(stream_.readEvent().isActive() || size() == 0){
			writePos_ += stream_.readSome(data_.get() + writePos_, std::min(std::max(dataSize, size()) - size(), capacity()));
			dispatcher_.set(size() > 0);
		}
		return std::min(size(), dataSize);
	}

	void BufferedStream::consume(std::size_t consumeSize){
		readPos_ += std::min(consumeSize, size());
		if(readPos_ == writePos_) readPos_ = writePos_ = 0;
		dispatcher_.set(size() > 0);
	}

}

