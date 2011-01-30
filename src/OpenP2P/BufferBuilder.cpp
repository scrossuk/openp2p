#include <stdint.h>
#include <algorithm>
#include <cstddef>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/BufferBuilder.hpp>
#include <OpenP2P/WaitHandler.hpp>

namespace OpenP2P{

	BufferBuilder::BufferBuilder(){ }

	std::size_t BufferBuilder::size(){
		return buffer_.size();
	}

	std::size_t BufferBuilder::writeSome(const uint8_t * data, std::size_t length, WaitHandler){
		buffer_ = buffer_ + Buffer(data, length);
		return length;
	}

	Buffer BufferBuilder::operator*() const{
		return buffer_;
	}

	IStream& operator>>(IStream& stream, Buffer& buffer){
		BufferBuilder builder;

		uint32_t bufferSize;
		stream >> bufferSize;

		uint8_t data[1024];
		std::size_t size;
		while((size = stream.readSome(data, std::min<uint32_t>(1024, bufferSize))) != 0){
			builder.write(data, size);
			bufferSize -= size;
		}

		buffer = *builder;
		return stream;
	}

}

