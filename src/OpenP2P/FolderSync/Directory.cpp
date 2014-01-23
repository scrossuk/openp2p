#include <stdint.h>
#include <string.h>

#include <array>
#include <stdexcept>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/Database.hpp>
#include <OpenP2P/FolderSync/Node.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		constexpr size_t ENTRY_NAME_SIZE = 16;
		
		constexpr size_t DIRECTORY_ENTRY_SIZE = BLOCK_ID_SIZE + ENTRY_NAME_SIZE;
		
		Directory::Directory(Node& node)
			: node_(node) { }
		
		Directory::~Directory() { }
		
		size_t Directory::childCount() const {
			return node_.size() / DIRECTORY_ENTRY_SIZE;
		}
		
		void Directory::addChild(const std::string& name, const BlockId& blockId) {
			(void) name;
			(void) blockId;
		}
		
		void Directory::removeChild(const std::string& name) {
			(void) name;
		}
		
		bool Directory::hasChild(const std::string& name) const {
			return false;
		}
		
		BlockId Directory::getChild(const std::string& name) const {
			throw std::runtime_error("Not implemented...");
		}
		
		std::vector<std::string> Directory::childNames() const {
			return std::vector<std::string>();
		}
		
	}
	
}

