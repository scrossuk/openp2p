#ifndef OPENP2P_FOLDERSYNC_BLOCKSTORE_HPP
#define OPENP2P_FOLDERSYNC_BLOCKSTORE_HPP

#include <stddef.h>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockPath.hpp>

namespace OpenP2P {
	
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
