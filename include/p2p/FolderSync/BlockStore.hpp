#ifndef P2P_FOLDERSYNC_BLOCKSTORE_HPP
#define P2P_FOLDERSYNC_BLOCKSTORE_HPP

#include <stddef.h>

#include <p2p/FolderSync/Block.hpp>
#include <p2p/FolderSync/BlockPath.hpp>

namespace p2p {
	
	namespace FolderSync {
		
		class BlockStore {
			public:
				virtual Block getRoot() const = 0;
				
				virtual void setRoot(Block block) = 0;
				
				virtual Block getBlock(const BlockPath& path, const Block& parentBlock) const = 0;
				
				virtual void setBlock(const BlockPath& path, Block& parentBlock, Block block) = 0;
			
		};
		
	}
	
}

#endif
