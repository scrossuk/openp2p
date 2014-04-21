#ifndef P2P_FOLDERSYNC_BLOCKCACHEMAP_HPP
#define P2P_FOLDERSYNC_BLOCKCACHEMAP_HPP

#include <stddef.h>

#include <unordered_map>

#include <p2p/FolderSync/Block.hpp>
#include <p2p/FolderSync/BlockPath.hpp>

namespace p2p {
	
	namespace FolderSync {
		
		class BlockStore;
		
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
				// Non-copyable.
				BlockCacheMap(const BlockCacheMap&) = delete;
				BlockCacheMap& operator=(BlockCacheMap) = delete;
				
				BlockStore& blockStore_;
				std::unordered_map<BlockPath, BlockInfo> blockMap_;
			
		};
		
	}
	
}

#endif
