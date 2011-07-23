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

	Future<Block> BufferIterator::readSome() {
		std::size_t maxReadSize = buffer_.size() - position_;
		std::size_t readSize = std::min(BlockSize, buffer_.size() - position_);

		MemBlock * memBlock = new MemBlock(readSize);
		for(std::size_t i = 0; i < readSize; i++, position_++){
			(*memBlock)[i] = buffer_[position_];
		}

		return Block(memBlock);
	}

}

