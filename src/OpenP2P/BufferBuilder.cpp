#include <stdint.h>
#include <cstddef>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/BufferBuilder.hpp>

namespace OpenP2P {

	BufferBuilder::BufferBuilder(Buffer& buffer) : buffer_(buffer) { }
	
	bool BufferBuilder::isValid() const {
		return true;
	}
	
	size_t BufferBuilder::write(const uint8_t* data, size_t size) {
		buffer_.reserve(buffer_.size() + size);
		buffer_.insert(buffer_.end(), data, data + size);
		return size;
	}
	
}

