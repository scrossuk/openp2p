#ifndef P2P_FOLDERSYNC_BLOCKCACHE_HPP
#define P2P_FOLDERSYNC_BLOCKCACHE_HPP

#include <p2p/FolderSync/BlockCacheMap.hpp>

namespace p2p {
	
	namespace FolderSync {
		
		class Block;
		class BlockPath;
		class BlockStore;
		
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
