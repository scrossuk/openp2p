#include <stdint.h>
#include <cstddef>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/BufferIterator.hpp>

namespace OpenP2P {

	BufferIterator::BufferIterator(const Buffer& buffer, std::size_t pPosition) : buffer_(buffer), position_(0) {
		seek(pPosition);
	}
	
	std::size_t BufferIterator::position() {
		return position_;
	}
	
	std::size_t BufferIterator::remaining() {
		return buffer_.size() - position_;
	}
	
	void BufferIterator::set(const Buffer& buffer) {
		buffer_ = buffer;
	}
	
	bool BufferIterator::seek(std::size_t pPosition) {
		if (pPosition > buffer_.size()) {
			return false;
		}
		
		position_ = pPosition;
		return true;
	}
	
	std::size_t BufferIterator::waitForData(Timeout) {
		return buffer_.size() - position_;
	}
	
	bool BufferIterator::read(uint8_t* data, std::size_t size, Timeout) {
		const std::size_t readSize = std::min(size, buffer_.size() - position_);
		
		for (std::size_t i = 0; i < readSize; i++) {
			data[i] = buffer_[position_ + i];
		}
		
		position_ += readSize;
		return true;
	}
	
}

