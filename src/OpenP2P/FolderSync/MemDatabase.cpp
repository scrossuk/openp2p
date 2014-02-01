#include <unordered_map>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/Database.hpp>
#include <OpenP2P/FolderSync/MemDatabase.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		MemDatabase::MemDatabase() {
			storeBlock(BlockId(), Block::Zero());
		}
		
		MemDatabase::~MemDatabase() { }
		
		bool MemDatabase::hasBlock(const BlockId& id) const {
			return data_.find(id) != data_.end();
		}
		
		Block MemDatabase::loadBlock(const BlockId& id) const {
			return data_.at(id).copy();
		}
		
		void MemDatabase::storeBlock(const BlockId& id, Block data) {
			data_.emplace(id, std::move(data));
		}
		
	}
	
}

