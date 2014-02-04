#include <assert.h>
#include <stdio.h>

#include <unordered_map>
#include <stdexcept>
#include <string>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/Database.hpp>
#include <OpenP2P/FolderSync/FileDatabase.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		namespace {
			
			class FILEHandle {
				public:
					inline FILEHandle()
						: handle_(nullptr) { }
					
					inline FILEHandle(const std::string& name)
						: handle_(nullptr) {
						handle_ = fopen(name.c_str(), "wb+");
						if (handle_ == nullptr) {
							throw std::runtime_error(std::string("Failed to open file '") + name + "'.");
						}
					}
					
					inline FILEHandle(FILEHandle&& file) noexcept
						: FILEHandle() {
						std::swap(handle_, file.handle_);
					}
					
					inline ~FILEHandle() {
						if (handle_ != nullptr) fclose(handle_);
					}
					
					FILEHandle& operator=(FILEHandle file) {
						std::swap(handle_, file.handle_);
						return *this;
					}
					
					inline size_t position() const {
						assert(handle_ != nullptr);
						const long result = ftell(handle_);
						assert(result >= 0);
						return static_cast<size_t>(result);
					}
					
					inline size_t size() const {
						assert(handle_ != nullptr);
						const size_t currentPosition = position();
						fseek(handle_, 0, SEEK_END);
						const size_t endPosition = position();
						fseek(handle_, static_cast<long>(currentPosition), SEEK_SET);
						return endPosition;
					}
					
					inline void seek(size_t seekPosition) {
						assert(handle_ != nullptr);
						fseek(handle_, static_cast<long>(seekPosition), SEEK_SET);
					}
					
					inline void seekEnd() {
						assert(handle_ != nullptr);
						fseek(handle_, 0, SEEK_END);
					}
					
					inline void readAll(uint8_t* data, size_t dataSize) {
						assert(handle_ != nullptr);
						const size_t result = fread(data, 1, dataSize, handle_);
						assert(result <= dataSize);
						if (result < dataSize) {
							if (ferror(handle_) != 0) {
								perror("Error");
							}
							throw std::runtime_error("File read failed.");
						}
					}
					
					inline void writeAll(const uint8_t* data, size_t dataSize) {
						assert(handle_ != nullptr);
						const size_t result = fwrite(data, 1, dataSize, handle_);
						assert(result <= dataSize);
						if (result < dataSize) {
							throw std::runtime_error("File write failed.");
						}
					}
					
				private:
					// Non-copyable.
					FILEHandle(const FILEHandle&) = delete;
					
					FILE* handle_;
				
			};
			
		}
		
		// Maximum number of blocks per database file.
		constexpr size_t MAX_DATABASE_FILE_BLOCKS = 65536;
		
		// Maximum number of bytes per database file.
		constexpr size_t MAX_DATABASE_FILE_BYTES = BLOCK_SIZE * MAX_DATABASE_FILE_BLOCKS;
		
		struct FilePosition {
			size_t fileNumber, blockIndex;
			
			inline FilePosition(size_t pFileNumber, size_t pBlockIndex)
				: fileNumber(pFileNumber), blockIndex(pBlockIndex) { }
		};
		
		struct FileDatabaseImpl {
			std::string path;
			std::unordered_map<BlockId, FilePosition> blockMap;
			std::unordered_map<size_t, FILEHandle> files;
			size_t currentFileNumber;
			
			inline FileDatabaseImpl(const std::string& pPath)
				: path(pPath), currentFileNumber(0) {
					files.emplace(currentFileNumber, FILEHandle(path + "/" + std::to_string(currentFileNumber) + ".block"));
				}
		};
		
		FileDatabase::FileDatabase(const std::string& path)
			: impl_(new FileDatabaseImpl(path)) {
			storeBlock(BlockId(), Block::Zero());
		}
		
		FileDatabase::~FileDatabase() { }
		
		Block FileDatabase::loadBlock(const BlockId& id) const {
			const auto& filePosition = impl_->blockMap.at(id);
			auto& blockFile = impl_->files.at(filePosition.fileNumber);
			
			blockFile.seek(filePosition.blockIndex * BLOCK_SIZE);
			
			auto block = Block::Zero();
			blockFile.readAll(block.data(), block.size());
			return std::move(block);
		}
		
		void FileDatabase::storeBlock(const BlockId& id, Block block) {
			if (impl_->blockMap.find(id) != impl_->blockMap.end()) {
				// Block already stored.
				return;
			}
			
			auto& currentFile = impl_->files.at(impl_->currentFileNumber);
			
			currentFile.seekEnd();
			
			assert((currentFile.position() % BLOCK_SIZE) == 0);
			assert(currentFile.position() < MAX_DATABASE_FILE_BYTES);
			
			// Record block position.
			impl_->blockMap.emplace(id, FilePosition(impl_->currentFileNumber, currentFile.position() / BLOCK_SIZE));
			
			// Write block.
			currentFile.writeAll(block.data(), block.size());
			
			assert((currentFile.position() % BLOCK_SIZE) == 0);
			assert(currentFile.position() <= MAX_DATABASE_FILE_BYTES);
			
			if (currentFile.position() == MAX_DATABASE_FILE_BYTES) {
				// Current database file is full; move to next.
				impl_->currentFileNumber++;
				impl_->files.emplace(impl_->currentFileNumber, FILEHandle(impl_->path + "/" + std::to_string(impl_->currentFileNumber) + ".block"));
			}
		}
		
	}
	
}

