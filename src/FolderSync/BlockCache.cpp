#include <p2p/FolderSync/Block.hpp>
#include <p2p/FolderSync/BlockCache.hpp>
#include <p2p/FolderSync/BlockCacheMap.hpp>
#include <p2p/FolderSync/BlockStore.hpp>
#include <p2p/FolderSync/Constants.hpp>

namespace p2p {

	namespace FolderSync {
	
		BlockCache::BlockCache(BlockStore& blockStore)
			: map_(blockStore) { }
			  
		BlockCache::~BlockCache() {
			flush();
		}
		
		const Block& BlockCache::getReadBlock(const BlockPath& path) const {
			return map_.loadBlock(path).block;
		}
		
		Block& BlockCache::getWriteBlock(const BlockPath& path) {
			auto& blockInfo = map_.loadBlock(path);
			blockInfo.isDirty = true;
			return blockInfo.block;
		}
		
		void BlockCache::flush() {
			map_.flush();
		}
		
	}
	
}

