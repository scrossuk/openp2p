#ifndef OPENP2P_FOLDERSYNC_NODE_HPP
#define OPENP2P_FOLDERSYNC_NODE_HPP

#include <vector>

#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/Database.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		class Node {
			public:
				Node(Database& database, const BlockId& blockId);
				~Node();
				
				BlockId findBlock(size_t offset);
				
			private:
				// ...
			
		};
		
	}
	
}

#endif
