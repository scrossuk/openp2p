#ifndef P2P_FOLDERSYNC_DATABASE_HPP
#define P2P_FOLDERSYNC_DATABASE_HPP

#include <p2p/FolderSync/Block.hpp>
#include <p2p/FolderSync/BlockId.hpp>

namespace p2p {
	
	namespace FolderSync {
		
		class Database {
			public:
				virtual Block loadBlock(const BlockId& id) const = 0;
				
				virtual void storeBlock(const BlockId& id, Block data) = 0;
			
		};
		
	}
	
}

#endif
