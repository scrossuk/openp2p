#include <vector>

#include <p2p/FolderSync/BlockId.hpp>
#include <p2p/FolderSync/Journal.hpp>

namespace p2p {
	
	namespace FolderSync {
		
		Journal::Journal() { }
		
		void Journal::addNewRoot(const BlockId& blockId) {
			entries_.push_back(blockId);
		}
		
		const BlockId& Journal::rootId() const {
			return entries_.back();
		}
		
	}
	
}

