#include <assert.h>
#include <stdint.h>

#include <stdexcept>
#include <unordered_map>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockCacheMap.hpp>
#include <OpenP2P/FolderSync/BlockStore.hpp>
#include <OpenP2P/FolderSync/Constants.hpp>

namespace OpenP2P {

	namespace FolderSync {
	
		BlockCacheMap::BlockCacheMap(BlockStore& blockStore)
			: blockStore_(blockStore) { }
			
		BlockInfo& BlockCacheMap::loadBlock(const BlockPath& path) {
			const auto iterator = blockMap_.find(path);
			if (iterator != blockMap_.end()) {
				return iterator->second;
			}
			
			if (path.isRoot()) {
				// Since root must be loaded before all other
				// blocks, the block map should be empty.
				assert(blockMap_.empty());
				
				return blockMap_.emplace(path, blockStore_.getRoot()).first->second;
			}
			
			// Must also load the parent block.
			auto& parentBlockInfo = loadBlock(path.parent());
			
			// Note that the parent has a new child; this
			// is also used to prevent it being evicted.
			parentBlockInfo.childCount++;
			
			assert(blockMap_.size() <= CACHE_MAX_BLOCKS);
			
			if (blockMap_.size() == CACHE_MAX_BLOCKS) {
				// Need to evict a block to make space
				// for this block.
				evictVictim();
			}
			
			// Load and insert the new block.
			auto block = blockStore_.getBlock(path, parentBlockInfo.block);
			return blockMap_.emplace(path, std::move(block)).first->second;
		}
		
		void BlockCacheMap::evictVictim() {
			assert(blockMap_.size() == CACHE_MAX_BLOCKS);
			
			for (auto iterator = blockMap_.begin(); iterator != blockMap_.end(); ++iterator) {
				const auto& blockPath = iterator->first;
				auto& blockInfo = iterator->second;
				
				if (blockInfo.childCount > 0) {
					// Only evict leaf blocks.
					continue;
				}
				
				// Root is parent of all blocks, so should never
				// be evicted to make space for another block.
				assert(!blockPath.isRoot());
				
				auto& parentInfo = blockMap_.at(blockPath.parent());
				
				// Parent has lost a child.
				parentInfo.childCount--;
				
				if (blockInfo.isDirty) {
					// If the victim was modified, store its latest
					// data in the block store.
					blockStore_.setBlock(blockPath, parentInfo.block, std::move(blockInfo.block));
					
					// Block store may have modified the parent.
					parentInfo.isDirty = true;
				}
				
				// Remove block from cache.
				blockMap_.erase(iterator);
				
				return;
			}
			
			throw std::runtime_error("Failed to find victim block.");
		}
		
		void BlockCacheMap::flush() {
			if (blockMap_.empty()) {
				return;
			}
			
			// To avoid flushing parent blocks before child blocks,
			// do each level in reverse depth order.
			for (size_t i = 1; i < NODE_MAX_BLOCK_PATH; i++) {
				const auto searchPathLength = NODE_MAX_BLOCK_PATH - i;
				
				for (auto& blockPair : blockMap_) {
					const auto& blockPath = blockPair.first;
					
					if (blockPath.size() != searchPathLength) {
						continue;
					}
					
					auto& blockInfo = blockPair.second;
					
					if (!blockInfo.isDirty) {
						continue;
					}
					
					auto& parentInfo = blockMap_.at(blockPath.parent());
					
					blockStore_.setBlock(blockPair.first, parentInfo.block, std::move(blockInfo.block));
					
					// Block store may have modified the parent.
					parentInfo.isDirty = true;
				}
			}
			
			// Also flush root.
			const auto rootPath = BlockPath::Root();
			auto& blockInfo = blockMap_.at(rootPath);
			blockStore_.setRoot(std::move(blockInfo.block));
			
			blockMap_.clear();
		}
		
	}
	
}

