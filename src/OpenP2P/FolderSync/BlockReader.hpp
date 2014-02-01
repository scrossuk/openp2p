#ifndef OPENP2P_FOLDERSYNC_BLOCKREADER_HPP
#define OPENP2P_FOLDERSYNC_BLOCKREADER_HPP

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <stdexcept>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/Constants.hpp>

#include <OpenP2P/Stream/BinaryStream.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		class BlockReader: public BlockingReader {
			public:
				inline BlockReader(const Block& block, size_t position = 0)
					: block_(block), position_(position) { }
				
				inline void seek(size_t position) {
					position_ = position;
				}
				
				inline void readAll(uint8_t* data, size_t size) {
					if ((position_ + size) > BLOCK_SIZE) {
						throw std::runtime_error("Block read overflowed.");
					}
					memcpy(data, block_.data() + position_, size);
					position_ += size;
				}
				
			private:
				const Block& block_;
				size_t position_;
				
		};
		
	}
	
}

#endif
