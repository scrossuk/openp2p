#ifndef P2P_FOLDERSYNC_BLOCK_HPP
#define P2P_FOLDERSYNC_BLOCK_HPP

#include <stdint.h>

#include <vector>

#include <p2p/FolderSync/Constants.hpp>

namespace p2p {
	
	namespace FolderSync {
		
		// Get number of blocks required to store a given number of bytes.
		inline size_t BYTES_TO_BLOCKS(size_t bytes) {
			return (bytes + (BLOCK_SIZE - 1)) / BLOCK_SIZE;
		}
		
		class Block {
			public:
				static Block Zero();
				
				Block(Block&& block) noexcept;
				
				uint8_t* data();
				
				const uint8_t* data() const;
				
				size_t size() const;
				
				Block copy() const;
				
			private:
				Block();
				
				// Non-copyable.
				Block(const Block&) = delete;
				Block& operator=(Block) = delete;
				
				std::vector<uint8_t> data_;
			
		};
		
	}
	
}

#endif
