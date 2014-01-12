#include <stdint.h>
#include <algorithm>
#include <cstddef>

#include <OpenP2P/Stream.hpp>

#include <OpenP2P/Buffer/BufferedStream.hpp>

namespace OpenP2P {

	BufferedStream::BufferedStream(IStream& stream, size_t pBufferSize)
		: stream_(stream), data_(new uint8_t[pBufferSize]),
		  bufferSize_(pBufferSize), readPos_(0), writePos_(0) { }
		  
	size_t BufferedStream::read(size_t dataSize) {
		// How much data should be read at maximum.
		const size_t desiredReadSize = std::max(dataSize, size()) - size();
		
		// How much data can be read (due to limited capacity).
		const size_t maxReadSize = std::min(capacity(), desiredReadSize);
		
		// Read any extra data from the stream.
		const size_t readSize = stream_.read(data_.get() + writePos_, maxReadSize);
		
		// Advance the write position.
		writePos_ += readSize;
		
		return size();
	}
	
	void BufferedStream::consume(size_t consumeSize) {
		readPos_ += std::min(consumeSize, size());
		
		if (readPos_ == writePos_) {
			readPos_ = writePos_ = 0;
		}
	}
	
}

