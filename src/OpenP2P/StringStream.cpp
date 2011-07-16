#include <stdint.h>
#include <cstddef>
#include <string>
#include <OpenP2P/StringStream.hpp>

namespace OpenP2P{

	StringIStream::StringIStream(const std::string& sourceString) : string_(sourceString), pos_(0){ }

	Future<std::size_t> StringIStream::readSome(uint8_t * data, std::size_t size){
		std::size_t readSize = std::min(size, string_.size() - pos_);
		for(std::size_t i = 0; i < readSize; i++){
			data[i] = string_[pos_ + i];
		}
		pos_ += readSize;
		return readSize;
	}

}

