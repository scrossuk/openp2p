#include <assert.h>
#include <stdint.h>
#include <string.h>

#include <array>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/Database.hpp>
#include <OpenP2P/FolderSync/Directory.hpp>
#include <OpenP2P/FolderSync/Node.hpp>
#include <OpenP2P/FolderSync/NodeReader.hpp>
#include <OpenP2P/FolderSync/NodeWriter.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		constexpr size_t ENTRY_NAME_SIZE = 48;
		
		constexpr size_t ENTRY_SIZE = ENTRY_NAME_SIZE + BLOCK_ID_SIZE;
		
		namespace {
			
			std::map<std::string, BlockId> readChildren(const Node& node) {
				std::map<std::string, BlockId> children;
				
				assert((node.size() % ENTRY_SIZE) == 0);
				
				const size_t childCount = node.size() / ENTRY_SIZE;
				
				NodeReader reader(node);
				for (size_t entryPos = 0; entryPos < childCount; entryPos++) {
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
				
				assert(node.size() == (children.size() * ENTRY_SIZE));
				
				return std::move(children);
			}
			
			void writeChildren(Node& node, const std::map<std::string, BlockId>& children) {
				node.resize(0);
				
				NodeWriter writer(node);
				for (const auto& child: children) {
					std::array<uint8_t, ENTRY_NAME_SIZE> entryNameData;
					entryNameData.fill(0x00);
					
					if (child.first.size() > ENTRY_NAME_SIZE) {
						throw std::runtime_error("Child name exceeds maximum name size.");
					}
					
					for (size_t i = 0; i < child.first.size(); i++) {
						entryNameData.at(i) = static_cast<uint8_t>(child.first.at(i));
					}
					
					writer.writeAll(entryNameData.data(), entryNameData.size());
					
					child.second.writeTo(writer);
				}
				
				printf("Node size: %llu.\n", (unsigned long long) node.size());
				printf("Children size: %llu.\n", (unsigned long long) children.size());
				
				assert(node.size() == (children.size() * ENTRY_SIZE));
			}
			
		}
		
		Directory::Directory(Node& node)
			: node_(node) { }
		
		Directory::~Directory() { }
		
		size_t Directory::childCount() const {
			return node_.size() / ENTRY_SIZE;
		}
		
		void Directory::addChild(const std::string& name, const BlockId& blockId) {
			if (name.empty()) {
				throw std::runtime_error("addChild(): Name is empty.");
			}
			
			if (name.size() > ENTRY_NAME_SIZE) {
				throw std::runtime_error("addChild(): Name is too long.");
			}
			
			auto children = readChildren(node_);
			
			if (children.find(name) != children.end()) {
				throw std::runtime_error("addChild(): Entry already exists.");
			}
			
			children.insert(std::make_pair(name, blockId));
			
			writeChildren(node_, children);
		}
		
		void Directory::forceAddChild(const std::string& name, const BlockId& blockId) {
			if (name.empty()) {
				throw std::runtime_error("forceAddChild(): Name is empty.");
			}
			
			if (name.size() > ENTRY_NAME_SIZE) {
				throw std::runtime_error("forceAddChild(): Name is too long.");
			}
			
			auto children = readChildren(node_);
			
			children[name] = blockId;
			
			writeChildren(node_, children);
		}
		
		void Directory::updateChild(const std::string& name, const BlockId& blockId) {
			if (name.empty()) {
				throw std::runtime_error("updateChild(): Name is empty.");
			}
			
			if (name.size() > ENTRY_NAME_SIZE) {
				throw std::runtime_error("updateChild(): Name is too long.");
			}
			
			auto children = readChildren(node_);
			
			if (children.find(name) == children.end()) {
				throw std::runtime_error("updateChild(): Entry does not already exist.");
			}
			
			children[name] = blockId;
			
			writeChildren(node_, children);
		}
		
		void Directory::removeChild(const std::string& name) {
			if (name.empty()) {
				throw std::runtime_error("removeChild(): Name is empty.");
			}
			
			if (name.size() > ENTRY_NAME_SIZE) {
				throw std::runtime_error("removeChild(): Name is too long.");
			}
			
			auto children = readChildren(node_);
			
			if (children.find(name) == children.end()) {
				throw std::runtime_error("removeChild(): Entry does not already exist.");
			}
			
			children.erase(name);
			
			writeChildren(node_, children);
		}
		
		bool Directory::hasChild(const std::string& name) const {
			if (name.empty()) {
				throw std::runtime_error("hasChild(): Name is empty.");
			}
			
			if (name.size() > ENTRY_NAME_SIZE) {
				throw std::runtime_error("hasChild(): Name is too long.");
			}
			
			const auto children = readChildren(node_);
			
			return children.find(name) != children.end();
		}
		
		BlockId Directory::getChild(const std::string& name) const {
			if (name.empty()) {
				throw std::runtime_error("getChild(): Name is empty.");
			}
			
			if (name.size() > ENTRY_NAME_SIZE) {
				throw std::runtime_error("getChild(): Name is too long.");
			}
			
			auto children = readChildren(node_);
			
			const auto iterator = children.find(name);
			
			if (iterator == children.end()) {
				throw std::runtime_error("getChild(): Entry does not already exist.");
			}
			
			return iterator->second;
		}
		
		std::vector<std::string> Directory::childNames() const {
			const auto children = readChildren(node_);
			
			std::vector<std::string> names;
			for (const auto& child: children) {
				names.push_back(child.first);
			}
			
			return names;
		}
		
	}
	
}

