#ifndef OPENP2P_FOLDERSYNC_NODEBLOCKSTORE_HPP
#define OPENP2P_FOLDERSYNC_NODEBLOCKSTORE_HPP

#include <stddef.h>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockStore.hpp>
#include <OpenP2P/FolderSync/Database.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		class NodeBlockStore: public BlockStore {
			public:
				NodeBlockStore(Database& database, Block& nodeBlock);
				~NodeBlockStore();
				
				Block getBlock(size_t position) const;
				
				void setBlock(size_t position, const Block& block);
				
				void unsetBlock(size_t position);
				
			private:
				// Non-copyable.
				NodeBlockStore(const NodeBlockStore&) = delete;
				NodeBlockStore& operator=(NodeBlockStore) = delete;
				
				Database& database_;
				Block& nodeBlock_;
				
		};
		
	}
	
}

#endif
