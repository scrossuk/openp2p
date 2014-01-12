#include <stdint.h>
#include <string.h>

#include <algorithm>
#include <string>

#include <OpenP2P/Stream/StringStream.hpp>

namespace OpenP2P {

	StringIStream::StringIStream(const std::string& sourceString) : string_(sourceString), position_(0) { }
	
	bool StringIStream::isValid() const {
		return string_.size() > position_;
	}
	
	size_t StringIStream::read(uint8_t* data, size_t size) {
		const size_t readSize = std::min(size, string_.size() - position_);
		
		memcpy(data, &(string_.c_str())[position_], readSize);
		position_ += readSize;
		
		return readSize;
	}
	
}

