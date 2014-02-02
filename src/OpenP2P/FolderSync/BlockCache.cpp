#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockCache.hpp>
#include <OpenP2P/FolderSync/BlockCacheMap.hpp>
#include <OpenP2P/FolderSync/BlockStore.hpp>
#include <OpenP2P/FolderSync/Constants.hpp>

namespace OpenP2P {

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

