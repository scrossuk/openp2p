#include <stdint.h>
#include <algorithm>
#include <cstddef>

#include <OpenP2P/BufferedStream.hpp>
#include <OpenP2P/EventHandle.hpp>
#include <OpenP2P/Stream.hpp>
#include <OpenP2P/Timeout.hpp>
#include <OpenP2P/TimeoutSequence.hpp>

namespace OpenP2P {

	BufferedStream::BufferedStream(InputStream& stream, std::size_t bufferSize)
		: stream_(stream), data_(new uint8_t[bufferSize]),
		  bufferSize_(bufferSize), readPos_(0), writePos_(0) { }
		  
	std::size_t BufferedStream::read(std::size_t dataSize, Timeout timeout) {
		TimeoutSequence sequence(timeout);
		
		// How much data should be read at maximum.
		const std::size_t maxReadSize = std::max(dataSize, size()) - size();
		
		// How much data can be read (due to limited capacity).
		const std::size_t maxSize = std::min(capacity(), maxReadSize);
		
		// How much data is available in the stream.
		const std::size_t availableSize = stream_.waitForData(sequence.getTimeout());
		
		// Either no data available, no data needs to be read, or the internal buffer is full.
		if (availableSize == 0 || maxSize == 0) {
			return size();
		}
		
		// How much data will now be read into the internal buffer.
		const std::size_t readSize = std::min(maxSize, availableSize);
		
		if (!stream_.read(data_.get() + writePos_, readSize, sequence.getTimeout())) {
			return size();
		}
		
		writePos_ += readSize;
		
		return size();
	}
	
	void BufferedStream::consume(std::size_t consumeSize) {
		readPos_ += std::min(consumeSize, size());
		
		if (readPos_ == writePos_) {
			readPos_ = writePos_ = 0;
		}
	}
	
}

