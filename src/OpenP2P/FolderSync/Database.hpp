#ifndef OPENP2P_FOLDERSYNC_DATABASE_HPP
#define OPENP2P_FOLDERSYNC_DATABASE_HPP

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockId.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		class Database {
			public:
				virtual Block loadBlock(const BlockId& id) const = 0;
				
				virtual void storeBlock(const BlockId& id, Block data) = 0;
			
		};
		
	}
	
}

#endif
