#include <stdint.h>

#include <map>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockStore.hpp>
#include <OpenP2P/FolderSync/WriteBuffer.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		constexpr size_t WRITE_BUFFER_MAX_SIZE = 2;
		
		WriteBuffer::WriteBuffer(BlockStore& blockStore)
			: blockStore_(blockStore) { }
		
		WriteBuffer::~WriteBuffer() {
			sync();
		}
		
		bool WriteBuffer::hasChanges() const {
			return !blockMap_.empty();
		}
		
		Block& WriteBuffer::getWriteBlock(size_t position) {
			const auto iterator = blockMap_.find(position);
			if (iterator != blockMap_.end()) {
				return iterator->second;
			}
			
			if (blockMap_.size() >= WRITE_BUFFER_MAX_SIZE) {
				// TODO: remove the least recently used block?
				const auto removeIterator = blockMap_.begin();
				blockStore_.setBlock(removeIterator->first, removeIterator->second);
				blockMap_.erase(removeIterator);
			}
			
			const auto insertResult = blockMap_.emplace(position, blockStore_.getBlock(position));
			const auto newIterator = insertResult.first;
			return newIterator->second;
		}
		
		void WriteBuffer::sync() {
			for (const auto& blockPair: blockMap_) {
				blockStore_.setBlock(blockPair.first, blockPair.second);
			}
			
			blockMap_.clear();
		}
		
	}
	
}

