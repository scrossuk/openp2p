#include <assert.h>
#include <stdio.h>

#include <stdexcept>
#include <string>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/Database.hpp>
#include <OpenP2P/FolderSync/FileDatabase.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		namespace {
			
			bool fileExists(const std::string& name) {
				FILE *file = fopen(name.c_str(), "r");
				if (file != nullptr) {
					fclose(file);
					return true;
				}
				return false;
			}
			
			class OpenFile {
				public:
					inline OpenFile(const std::string& name, bool openForWrite)
						: handle_(nullptr) {
						handle_ = fopen(name.c_str(), openForWrite ? "wb" : "rb");
						if (handle_ == nullptr) {
							throw std::runtime_error(std::string("Failed to open file '") + name + "'.");
						}
					}
					
					inline ~OpenFile() {
						fclose(handle_);
					}
					
					inline void readAll(uint8_t* data, size_t dataSize) {
						const size_t result = fread(data, 1, dataSize, handle_);
						assert(result <= dataSize);
						if (result < dataSize) {
							throw std::runtime_error("File read failed.");
						}
					}
					
					inline void writeAll(const uint8_t* data, size_t dataSize) {
						const size_t result = fwrite(data, 1, dataSize, handle_);
						assert(readResult <= dataSize);
						if (result < dataSize) {
							throw std::runtime_error("File write failed.");
						}
					}
					
				private:
					// Non-copyable.
					OpenFile(const OpenFile&) = delete;
					OpenFile& operator=(OpenFile) = delete;
					
					FILE* handle_;
				
			};
			
		}
		
		FileDatabase::FileDatabase(const std::string& path)
			: path_(path) {
			storeBlock(BlockId(), Block::Zero());
		}
		
		FileDatabase::~FileDatabase() { }
		
		Block FileDatabase::loadBlock(const BlockId& id) const {
			OpenFile openFile(path_ + "/" + id.hexString() + ".block", false);
			auto block = Block::Zero();
			openFile.readAll(block.data(), block.size());
			return std::move(block);
		}
		
		void FileDatabase::storeBlock(const BlockId& id, Block block) {
			const std::string fileName = path_ + "/" + id.hexString() + ".block";
			if (fileExists(fileName)) return;
			
			OpenFile openFile(fileName, true);
			openFile.writeAll(block.data(), block.size());
		}
		
	}
	
}

