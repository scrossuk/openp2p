#include <stdint.h>
#include <string.h>

#include <OpenP2P/Buffer/Buffer.hpp>
#include <OpenP2P/Buffer/BufferIterator.hpp>

namespace OpenP2P {

	BufferIterator::BufferIterator(const Buffer& buffer, size_t pPosition) : buffer_(buffer), position_(0) {
		seek(pPosition);
	}
	
	size_t BufferIterator::position() const {
		return position_;
	}
	
	size_t BufferIterator::remaining() const {
		return buffer_.size() - position_;
	}
	
	void BufferIterator::set(const Buffer& buffer) {
		buffer_ = buffer;
		position_ = 0;
	}
	
	bool BufferIterator::seek(size_t pPosition) {
		if (pPosition > buffer_.size()) {
			return false;
		}
		
		position_ = pPosition;
		return true;
	}
	
	bool BufferIterator::isValid() const {
		return remaining() > 0;
	}
	
	Event::Source BufferIterator::eventSource() const {
		return Event::Source();
	}
	
	size_t BufferIterator::read(uint8_t* data, size_t size) {
		const size_t readSize = std::min(size, remaining());
		
		memcpy(data, &buffer_[position_], readSize);
		position_ += readSize;
		
		return readSize;
	}
	
}

