#include <unordered_map>

#include <p2p/FolderSync/Block.hpp>
#include <p2p/FolderSync/BlockId.hpp>
#include <p2p/FolderSync/Database.hpp>
#include <p2p/FolderSync/MemDatabase.hpp>

namespace p2p {
	
	namespace FolderSync {
		
		MemDatabase::MemDatabase() {
			storeBlock(BlockId(), Block::Zero());
		}
		
		MemDatabase::~MemDatabase() { }
		
		Block MemDatabase::loadBlock(const BlockId& id) const {
			return data_.at(id).copy();
		}
		
		void MemDatabase::storeBlock(const BlockId& id, Block data) {
			data_.emplace(id, std::move(data));
		}
		
	}
	
}

