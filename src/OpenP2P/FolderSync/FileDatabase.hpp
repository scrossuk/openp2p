#ifndef OPENP2P_FOLDERSYNC_FILEDATABASE_HPP
#define OPENP2P_FOLDERSYNC_FILEDATABASE_HPP

#include <memory>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/Database.hpp>

namespace OpenP2P {
	
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
