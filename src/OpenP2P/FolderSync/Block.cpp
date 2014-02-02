#include <stdint.h>

#include <vector>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/Constants.hpp>

namespace OpenP2P {

	namespace FolderSync {
	
		Block Block::Zero() {
			Block block;
			block.data_.resize(BLOCK_SIZE, 0x00);
			return std::move(block);
		}
		
		Block::Block(Block&& block) noexcept
			: data_(std::move(block.data_)) { }
		
		uint8_t* Block::data() {
			return data_.data();
		}
		
		const uint8_t* Block::data() const {
			return data_.data();
		}
		
		size_t Block::size() const {
			return BLOCK_SIZE;
		}
		
		Block Block::copy() const {
			Block copyBlock;
			copyBlock.data_ = data_;
			return std::move(copyBlock);
		}
		
		Block::Block() { }
		
	}
	
}

