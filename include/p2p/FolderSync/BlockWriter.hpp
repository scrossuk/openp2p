#ifndef P2P_FOLDERSYNC_BLOCKWRITER_HPP
#define P2P_FOLDERSYNC_BLOCKWRITER_HPP

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <stdexcept>

#include <p2p/FolderSync/Block.hpp>
#include <p2p/FolderSync/Constants.hpp>

#include <p2p/Transport.hpp>

namespace p2p {
	
	namespace FolderSync {
		
		class BlockWriter: public BlockingWriter {
			public:
				inline BlockWriter(Block& block, size_t position = 0)
					: block_(block), position_(position) { }
				
				inline void seek(size_t position) {
					position_ = position;
				}
				
				inline void writeAll(const uint8_t* data, size_t size) {
					if ((position_ + size) > BLOCK_SIZE) {
						throw std::runtime_error("Block write overflowed.");
					}
					memcpy(block_.data() + position_, data, size);
					position_ += size;
				}
				
			private:
				Block& block_;
				size_t position_;
				
		};
		
	}
	
}

#endif
