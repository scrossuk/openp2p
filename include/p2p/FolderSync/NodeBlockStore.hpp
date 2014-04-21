#ifndef P2P_FOLDERSYNC_NODEBLOCKSTORE_HPP
#define P2P_FOLDERSYNC_NODEBLOCKSTORE_HPP

#include <stddef.h>

#include <p2p/FolderSync/Block.hpp>
#include <p2p/FolderSync/BlockPath.hpp>
#include <p2p/FolderSync/BlockStore.hpp>
#include <p2p/FolderSync/Database.hpp>

namespace p2p {
	
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
