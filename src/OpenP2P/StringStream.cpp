#include <stdint.h>
#include <cstddef>
#include <string>
#include <OpenP2P/StringStream.hpp>

namespace OpenP2P{

	StringIStream::StringIStream(const std::string& sourceString) : string_(sourceString), pos_(0){ }

	Future<Block> StringIStream::readSome(){
		std::size_t readSize = std::min(BlockSize, string_.size() - pos_);
		
		MemBlock * memBlock = new MemBlock(readSize);
		for(std::size_t i = 0; i < readSize; i++){
			(*memBlock)[i] = string_[pos_ + i];
		}
		pos_ += readSize;
		return Block(memBlock);
	}

}

