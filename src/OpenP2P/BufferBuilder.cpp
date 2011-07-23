#include <stdint.h>
#include <cstddef>
#include <vector>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/BufferBuilder.hpp>

namespace OpenP2P{

	BufferBuilder::BufferBuilder(Buffer& buffer) : buffer_(buffer){ }

	Future<std::size_t> BufferBuilder::writeSome(const Block& block){
		buffer_.reserve(buffer_.size() + block.size());
		buffer_.insert(buffer_.end(), block.get(), block.get() + block.size());
		return block.size();
	}

}

