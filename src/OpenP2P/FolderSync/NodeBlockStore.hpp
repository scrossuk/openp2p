#ifndef OPENP2P_FOLDERSYNC_NODEBLOCKSTORE_HPP
#define OPENP2P_FOLDERSYNC_NODEBLOCKSTORE_HPP

#include <stddef.h>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockPath.hpp>
#include <OpenP2P/FolderSync/BlockStore.hpp>
#include <OpenP2P/FolderSync/Database.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		inline size_t NodeBlockIdOffset(const BlockPath& path) {
			const bool isDirectRef = path.size() == 1;
			return (isDirectRef ? NODE_BLOCK_ID_OFFSET : 0) + path.back() * BLOCK_ID_SIZE;
		}
		
		class NodeBlockStore: public BlockStore {
			public:
				NodeBlockStore(Database& database, const BlockId& rootId);
				~NodeBlockStore();
				
				const BlockId& rootId() const;
				
				Block getRoot() const;
				
				void setRoot(Block block);
				
				Block getBlock(const BlockPath& path, const Block& parentBlock) const;
				
				void setBlock(const BlockPath& path, Block& parentBlock, Block block);
				
			private:
				// Non-copyable.
				NodeBlockStore(const NodeBlockStore&) = delete;
				NodeBlockStore& operator=(NodeBlockStore) = delete;
				
				Database& database_;
				BlockId rootId_;
				
		};
		
	}
	
}

#endif
