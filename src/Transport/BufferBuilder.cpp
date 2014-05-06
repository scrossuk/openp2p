#include <stdint.h>

#include <p2p/Transport/Buffer.hpp>
#include <p2p/Transport/BufferBuilder.hpp>

namespace p2p {

	BufferBuilder::BufferBuilder(Buffer& buffer) : buffer_(buffer) { }
	
	bool BufferBuilder::isValid() const {
		return true;
	}
	
	Event::Source BufferBuilder::eventSource() const {
		return Event::Source();
	}
	
	size_t BufferBuilder::write(const uint8_t* data, size_t size) {
		buffer_.reserve(buffer_.size() + size);
		buffer_.insert(buffer_.end(), data, data + size);
		return size;
	}
	
}

