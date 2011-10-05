#include <stdint.h>
#include <cstddef>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/BufferBuilder.hpp>

namespace OpenP2P{

	BufferBuilder::BufferBuilder(Buffer& buffer) : buffer_(buffer){ }
	
	std::size_t BufferBuilder::waitForSpace(Timeout){
		return buffer_.max_size() - buffer_.size();
	}

	bool BufferBuilder::write(const uint8_t * data, std::size_t size, Timeout){
		buffer_.reserve(buffer_.size() + size);
		buffer_.insert(buffer_.end(), data, data + size);
		return true;
	}

}

