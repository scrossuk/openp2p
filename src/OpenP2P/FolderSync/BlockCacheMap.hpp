#ifndef OPENP2P_FOLDERSYNC_BLOCKCACHEMAP_HPP
#define OPENP2P_FOLDERSYNC_BLOCKCACHEMAP_HPP

#include <stddef.h>

#include <unordered_map>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockPath.hpp>

namespace OpenP2P {
	
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
