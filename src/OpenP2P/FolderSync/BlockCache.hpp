#ifndef OPENP2P_FOLDERSYNC_BLOCKCACHE_HPP
#define OPENP2P_FOLDERSYNC_BLOCKCACHE_HPP

#include <stddef.h>

#include <unordered_map>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockStore.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		class BlockCache {
			public:
				BlockCache(BlockStore& blockStore);
				~BlockCache();
				
				const Block& getReadBlock(size_t index) const;
				
				Block& getWriteBlock(size_t index);
				
				void flush();
				
			private:
				// Non-copyable.
				BlockCache(const BlockCache&) = delete;
				BlockCache& operator=(BlockCache) = delete;
				
				BlockStore& blockStore_;
				
				struct BlockInfo {
					bool isDirty;
					Block block;
					
					inline BlockInfo(bool pIsDirty, Block&& pBlock)
						: isDirty(pIsDirty), block(std::move(pBlock)) { }
				};
				
				mutable std::unordered_map<size_t, BlockInfo> blockMap_;
				
		};
		
	}
	
}

#endif
