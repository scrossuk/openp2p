#include <stdint.h>
#include <cstddef>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/BufferIterator.hpp>

namespace OpenP2P {

	BufferIterator::BufferIterator(const Buffer& buffer, std::size_t position) : buffer_(buffer), position_(0) {
		seek(position);
	}

	std::size_t BufferIterator::position() {
		return position_;
	}

	std::size_t BufferIterator::remaining() {
		return buffer_.size() - position_;
	}

	void BufferIterator::set(const Buffer& buffer){
		buffer_ = buffer;
	}

	bool BufferIterator::seek(std::size_t position) {
		if (position > buffer_.size()) {
			return false;
		}

		position_ = position;
		return true;
	}
	
	EventHandle BufferIterator::readEvent(){
		return EventHandle::True;
	}

	std::size_t BufferIterator::readSome(uint8_t * data, std::size_t dataSize){
		const std::size_t readSize = std::min(dataSize, buffer_.size() - position_);

		for(std::size_t i = 0; i < readSize; i++){
			data[i] = buffer_[position_ + i];
		}
		
		position_ += readSize;

		return dataSize;
	}

}

