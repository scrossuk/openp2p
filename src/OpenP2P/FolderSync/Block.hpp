#ifndef OPENP2P_FOLDERSYNC_BLOCK_HPP
#define OPENP2P_FOLDERSYNC_BLOCK_HPP

#include <stdint.h>

#include <vector>

#include <OpenP2P/FolderSync/Constants.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		// Get number of blocks required to store a given number of bytes.
		inline size_t BYTES_TO_BLOCKS(size_t bytes) {
			return (bytes + (BLOCK_SIZE - 1)) / BLOCK_SIZE;
		}
		
		class Block {
			public:
				static inline Block Zero() {
					return Block();
				}
				
				inline Block(Block&& block) noexcept
					: data_(block.data_) { }
				
				inline uint8_t* data() {
					return data_.data();
				}
				
				inline const uint8_t* data() const {
					return data_.data();
				}
				
				inline size_t size() const {
					return BLOCK_SIZE;
				}
				
				inline Block copy() const {
					Block copyBlock;
					copyBlock.data_ = data_;
					return std::move(copyBlock);
				}
				
			private:
				inline Block() : data_(BLOCK_SIZE, 0x00) { }
				
				// Non-copyable.
				Block(const Block&) = delete;
				Block& operator=(Block) = delete;
				
				std::vector<uint8_t> data_;
			
		};
		
	}
	
}

#endif
