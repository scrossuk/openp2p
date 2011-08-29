#include <stdint.h>
#include <algorithm>
#include <cstddef>
#include <string>
#include <OpenP2P/StringStream.hpp>

namespace OpenP2P{

	StringIStream::StringIStream(const std::string& sourceString) : string_(sourceString), pos_(0){ }
	
	EventHandle StringIStream::readEvent(){
		return EventHandle::True;
	}

	std::size_t StringIStream::readSome(uint8_t * data, std::size_t dataSize){
		std::size_t readSize = std::min(dataSize, string_.size() - pos_);
		
		for(std::size_t i = 0; i < readSize; i++){
			data[i] = string_[pos_ + i];
		}
		pos_ += readSize;
		
		return readSize;
	}

}

