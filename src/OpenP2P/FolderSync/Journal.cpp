#include <vector>

#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/Journal.hpp>

namespace OpenP2P {
	
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

