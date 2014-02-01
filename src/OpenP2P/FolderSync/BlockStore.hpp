#ifndef OPENP2P_FOLDERSYNC_BLOCKSTORE_HPP
#define OPENP2P_FOLDERSYNC_BLOCKSTORE_HPP

#include <stddef.h>

#include <OpenP2P/FolderSync/Block.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		class BlockStore {
			public:
				virtual Block getBlock(size_t index) const = 0;
				
				virtual void setBlock(size_t index, Block block) = 0;
			
		};
		
	}
	
}

#endif
