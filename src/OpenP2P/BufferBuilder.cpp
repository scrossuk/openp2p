#include <stdint.h>
#include <algorithm>
#include <cstddef>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/BufferBuilder.hpp>

namespace OpenP2P{

	BufferBuilder::BufferBuilder(){ }

	std::size_t BufferBuilder::size(){
		return buffer_.size();
	}

	std::size_t BufferBuilder::writeSome(const uint8_t * data, std::size_t length){
		buffer_ = buffer_ + Buffer(data, length);
		return length;
	}

	void BufferBuilder::cancel(){ }

	Buffer BufferBuilder::getBuffer() const{
		return buffer_;
	}

}

