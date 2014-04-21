#ifndef P2P_FOLDERSYNC_JOURNAL_HPP
#define P2P_FOLDERSYNC_JOURNAL_HPP

#include <vector>

#include <p2p/FolderSync/BlockId.hpp>

namespace p2p {
	
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
