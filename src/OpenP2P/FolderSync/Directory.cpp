#include <stdint.h>
#include <string.h>

#include <array>
#include <stdexcept>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/Database.hpp>
#include <OpenP2P/FolderSync/Directory.hpp>
#include <OpenP2P/FolderSync/Node.hpp>
#include <OpenP2P/FolderSync/NodeReader.hpp>
#include <OpenP2P/FolderSync/NodeWriter.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		constexpr size_t ENTRY_NAME_SIZE = 16;
		
		constexpr size_t ENTRY_SIZE = ENTRY_NAME_SIZE + BLOCK_ID_SIZE;
		
		Directory::Directory(Node& node)
			: node_(node) { }
		
		Directory::~Directory() { }
		
		size_t Directory::childCount() const {
			return node_.size() / ENTRY_SIZE;
		}
		
		void Directory::addChild(const std::string& name, const BlockId& blockId) {
			if (name.empty()) {
				throw std::runtime_error("Name is empty.");
			}
			
			if (name.size() > ENTRY_NAME_SIZE) {
				throw std::runtime_error("Name is too long.");
			}
			
			std::array<uint8_t, ENTRY_NAME_SIZE> nameData;
			nameData.fill(0x00);
			for (size_t i = 0; i < name.size(); i++) {
				nameData.at(i) = (uint8_t) name.at(i);
			}
			
			std::map<std::string, BlockId> children;
			
			NodeReader reader(node_);
			for (size_t entryPos = 0; entryPos < childCount(); entryPos++) {
				std::array<uint8_t, ENTRY_NAME_SIZE> entryNameData;
				reader.readAll(entryNameData.data(), entryNameData.size());
				
				std::string entryName;
				for (size_t i = 0; i < entryNameData.size(); i++) {
					if (entryNameData.at(i) == 0x00) break;
					entryName += (char) entryNameData.at(i);
				}
				
				const auto entryBlockId = BlockId::FromReader(reader);
				
				children.insert(std::make_pair(entryName, entryBlockId));
			}
			
			if (children.find(name) != children.end()) {
				throw std::runtime_error("Entry already exists.");
			}
			
			children[name] = blockId;
			
			NodeWriter writer(node_);
			for (const auto& child: children) {
				std::array<uint8_t, ENTRY_NAME_SIZE> entryNameData;
				reader.writeAll(entryNameData.data(), entryNameData.size());
				
				std::string entryName;
				for (size_t i = 0; i < entryNameData.size(); i++) {
					if (entryNameData.at(i) == 0x00) break;
					entryName += (char) entryNameData.at(i);
				}
				
				const auto entryBlockId = BlockId::FromReader(reader);
				
				children.insert(std::make_pair(entryName, entryBlockId));
			}
		}
		
		void Directory::updateChild(const std::string& name, const BlockId& blockId) {
			(void) name;
			(void) blockId;
		}
		
		void Directory::removeChild(const std::string& name) {
			(void) name;
		}
		
		bool Directory::hasChild(const std::string& name) const {
			(void) name;
			return false;
		}
		
		BlockId Directory::getChild(const std::string& name) const {
			(void) name;
			throw std::runtime_error("Not implemented...");
		}
		
		std::vector<std::string> Directory::childNames() const {
			return std::vector<std::string>();
		}
		
	}
	
}

