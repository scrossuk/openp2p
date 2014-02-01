#ifndef OPENP2P_FOLDERSYNC_BLOCKCACHE_HPP
#define OPENP2P_FOLDERSYNC_BLOCKCACHE_HPP

#include <stddef.h>

#include <unordered_map>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockPath.hpp>
#include <OpenP2P/FolderSync/BlockStore.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		struct BlockInfo {
			bool isDirty;
			size_t childCount;
			Block block;
			
			inline BlockInfo(Block&& pBlock)
				: isDirty(false),
				childCount(0),
				block(std::move(pBlock)) { }
		};
		
		class BlockCacheMap {
			public:
				BlockCacheMap(BlockStore& blockStore);
				
				BlockInfo& loadBlock(const BlockPath& path);
				
				void evictVictim();
				
				void flush();
				
			private:
				BlockStore& blockStore_;
				std::unordered_map<BlockPath, BlockInfo> blockMap_;
			
		};
		
		class BlockCache {
			public:
				BlockCache(BlockStore& blockStore);
				~BlockCache();
				
				const Block& getReadBlock(const BlockPath& path) const;
				
				Block& getWriteBlock(const BlockPath& path);
				
				void flush();
				
			private:
				// Non-copyable.
				BlockCache(const BlockCache&) = delete;
				BlockCache& operator=(BlockCache) = delete;
				
				mutable BlockCacheMap map_;
				
		};
		
	}
	
}

#endif
