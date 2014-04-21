#ifndef P2P_FOLDERSYNC_FILEDATABASE_HPP
#define P2P_FOLDERSYNC_FILEDATABASE_HPP

#include <memory>

#include <p2p/FolderSync/Block.hpp>
#include <p2p/FolderSync/BlockId.hpp>
#include <p2p/FolderSync/Database.hpp>

namespace p2p {
	
	namespace FolderSync {
		
		class FileDatabase: public Database {
			public:
				FileDatabase(const std::string& path);
				~FileDatabase();
				
				Block loadBlock(const BlockId& id) const;
				
				void storeBlock(const BlockId& id, Block data);
				
			private:
				// Non-copyable.
				FileDatabase(const FileDatabase&) = delete;
				FileDatabase& operator=(FileDatabase) = delete;
				
				std::unique_ptr<struct FileDatabaseImpl> impl_;
			
		};
		
	}
	
}

#endif
