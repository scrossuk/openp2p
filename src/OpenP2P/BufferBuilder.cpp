#include <stdint.h>
#include <cstddef>
#include <vector>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/BufferBuilder.hpp>

namespace OpenP2P{

	BufferBuilder::BufferBuilder(Buffer& buffer) : buffer_(buffer){ }
	
	EventHandle BufferBuilder::writeEvent(){
		return EventHandle::True;
	}

	std::size_t BufferBuilder::writeSome(const uint8_t * data, std::size_t dataSize){
		buffer_.reserve(buffer_.size() + dataSize);
		buffer_.insert(buffer_.end(), data, data + dataSize);
		return dataSize;
	}

}

