#include <stdint.h>
#include <algorithm>
#include <cstddef>
#include <string>
#include <OpenP2P/StringStream.hpp>

namespace OpenP2P{

	StringIStream::StringIStream(const std::string& sourceString) : string_(sourceString), pos_(0){ }
	
	std::size_t StringIStream::waitForData(Timeout){
		return string_.size() - pos_;
	}

	bool StringIStream::read(uint8_t * data, std::size_t size, Timeout){
		if(size > (string_.size() - pos_)){
			return false;
		}
		
		for(std::size_t i = 0; i < size; i++){
			data[i] = string_[pos_ + i];
		}
		
		pos_ += size;
		
		return true;
	}

}

