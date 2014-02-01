#include <assert.h>
#include <stdint.h>

#include <stdexcept>
#include <unordered_map>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockCache.hpp>
#include <OpenP2P/FolderSync/BlockStore.hpp>
#include <OpenP2P/FolderSync/Constants.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		namespace {
			
			// TODO: move this into its own class.
			template <typename Container>
			void evictVictim(Container& container, BlockStore& blockStore) {
				if (container.size() < CACHE_MAX_BLOCKS) return;
				
				// TODO: remove the least recently used block?
				const auto removeIterator = container.begin();
				
				auto& blockInfo = removeIterator->second;
				
				if (blockInfo.isDirty) {
					blockStore.setBlock(removeIterator->first, std::move(blockInfo.block));
				}
				
				container.erase(removeIterator);
			}
			
		}
		
		BlockCache::BlockCache(BlockStore& blockStore)
			: blockStore_(blockStore) { }
		
		BlockCache::~BlockCache() {
			flush();
		}
		
		const Block& BlockCache::getReadBlock(size_t index) const {
			const auto iterator = blockMap_.find(index);
			if (iterator != blockMap_.end()) {
				const BlockInfo& blockInfo = iterator->second;
				return blockInfo.block;
			}
			
			evictVictim(blockMap_, blockStore_);
			
			const auto insertResult = blockMap_.emplace(index, BlockInfo(false, blockStore_.getBlock(index)));
			const auto newIterator = insertResult.first;
			return newIterator->second.block;
		}
		
		Block& BlockCache::getWriteBlock(size_t index) {
			const auto iterator = blockMap_.find(index);
			if (iterator != blockMap_.end()) {
				BlockInfo& blockInfo = iterator->second;
				
				// Mark block as dirty.
				blockInfo.isDirty = true;
				
				return blockInfo.block;
			}
			
			evictVictim(blockMap_, blockStore_);
			
			const auto insertResult = blockMap_.emplace(index, BlockInfo(true, blockStore_.getBlock(index)));
			const auto newIterator = insertResult.first;
			return newIterator->second.block;
		}
		
		void BlockCache::flush() {
			for (auto& blockPair: blockMap_) {
				auto& blockInfo = blockPair.second;
				if (!blockInfo.isDirty) continue;
				blockStore_.setBlock(blockPair.first, std::move(blockInfo.block));
			}
			
			blockMap_.clear();
		}
		
	}
	
}

