#ifndef OPENP2P_FOLDERSYNC_BLOCKSTORE_HPP
#define OPENP2P_FOLDERSYNC_BLOCKSTORE_HPP

#include <stddef.h>

#include <OpenP2P/FolderSync/Block.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		class BlockStore {
			public:
				virtual Block getBlock(size_t position) const = 0;
				
				virtual void setBlock(size_t position, const Block& block) = 0;
			
		};
		
	}
	
}

#endif
