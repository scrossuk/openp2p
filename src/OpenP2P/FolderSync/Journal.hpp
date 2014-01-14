#ifndef OPENP2P_FOLDERSYNC_JOURNAL_HPP
#define OPENP2P_FOLDERSYNC_JOURNAL_HPP

#include <vector>

#include <OpenP2P/FolderSync/BlockId.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		class Journal {
			public:
				Journal();
				
				void addNewRoot(const BlockId& blockId);
				
				const BlockId& rootId() const;
				
			private:
				std::vector<BlockId> entries_;
			
		};
		
	}
	
}

#endif
