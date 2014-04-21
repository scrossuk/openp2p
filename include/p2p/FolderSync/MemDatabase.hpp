#ifndef P2P_FOLDERSYNC_MEMDATABASE_HPP
#define P2P_FOLDERSYNC_MEMDATABASE_HPP

#include <unordered_map>

#include <p2p/FolderSync/Block.hpp>
#include <p2p/FolderSync/BlockId.hpp>
#include <p2p/FolderSync/Database.hpp>

namespace p2p {
	
	namespace FolderSync {
		
		class MemDatabase: public Database {
			public:
				MemDatabase();
				~MemDatabase();
				
				Block loadBlock(const BlockId& id) const;
				
				void storeBlock(const BlockId& id, Block data);
				
			private:
				std::unordered_map<BlockId, Block> data_;
			
		};
		
	}
	
}

#endif
