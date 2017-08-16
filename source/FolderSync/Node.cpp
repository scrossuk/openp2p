#include <stdint.h>
#include <string.h>

#include <array>
#include <stdexcept>

#include <p2p/FolderSync/Block.hpp>
#include <p2p/FolderSync/BlockId.hpp>
#include <p2p/FolderSync/BlockReader.hpp>
#include <p2p/FolderSync/BlockWriter.hpp>
#include <p2p/FolderSync/Constants.hpp>
#include <p2p/FolderSync/Database.hpp>
#include <p2p/FolderSync/Node.hpp>

#include <p2p/Transport.hpp>

namespace p2p {
	
	namespace FolderSync {
		
		BlockId CreateEmptyNode(Database& database, NodeType type) {
			auto emptyBlock = Block::Zero();
			
			BlockWriter writer(emptyBlock);
			
			writer.seek(NODE_SIZE_OFFSET);
			Binary::WriteUint64(writer, 0);
			
			writer.seek(NODE_TYPE_OFFSET);
			Binary::WriteUint8(writer, type == TYPE_DIRECTORY ? 0 : 1);
			
			const auto emptyBlockId = BlockId::Generate(emptyBlock);
			database.storeBlock(emptyBlockId, std::move(emptyBlock));
			
			return emptyBlockId;
		}
		
		Node::Node(Database& database, const BlockId& initialBlockId)
			: hasChanged_(false),
			blockStore_(database, initialBlockId),
			cache_(blockStore_),
			size_(0),
			type_(TYPE_DIRECTORY) {
				BlockReader reader(cache_.getReadBlock(BlockPath::Root()));
				
				reader.seek(NODE_SIZE_OFFSET);
				size_ = Binary::ReadUint64(reader);
				
				reader.seek(NODE_TYPE_OFFSET);
				const uint8_t typeVal = Binary::ReadUint8(reader);
				if (typeVal == 0) {
					type_ = TYPE_DIRECTORY;
				} else {
					type_ = TYPE_FILE;
				}
			}
		
		Node::~Node() {
			flush();
		}
		
		BlockId Node::blockId() const {
			return blockStore_.rootId();
		}
		
		NodeSize Node::size() const {
			return size_;
		}
		
		NodeType Node::type() const {
			return type_;
		}
		
		void Node::flush() {
			cache_.flush();
		}
		
		bool Node::hasChanged() const {
			return hasChanged_;
		}
		
		void Node::resize(NodeSize newSize) {
			if (size() == newSize) return;
			
			hasChanged_ = true;
			
			// Flush all data blocks, so that IDs of
			// deleted blocks aren't updated incorrectly
			// at some later time.
			cache_.flush();
			
			const size_t oldBlockCount = BYTES_TO_BLOCKS(size());
			const size_t newBlockCount = BYTES_TO_BLOCKS(newSize);
			
			// Set the size field.
			BlockWriter sizeWriter(cache_.getWriteBlock(BlockPath::Root()), NODE_SIZE_OFFSET);
			Binary::WriteUint64(sizeWriter, newSize);
			
			size_ = newSize;
			
			// Replace block IDs with zero for deleted blocks.
			if (oldBlockCount > newBlockCount) {
				const auto zeroId = BlockId::Zero();
				
				for (size_t i = newBlockCount; i < oldBlockCount; i++) {
					const auto path = BlockPath::Index(i);
					auto& parentBlock = cache_.getWriteBlock(path.parent());
					BlockWriter writer(parentBlock, NodeBlockIdOffset(path));
					zeroId.writeTo(writer);
				}
			}
		}
		
		size_t Node::read(NodeOffset offset, uint8_t* buffer, size_t bufferSize) const {
			if (offset > size()) {
				throw std::runtime_error("Offset exceeds size.");
			}
			
			const size_t readSize = std::min<size_t>(bufferSize, size() - offset);
			
			for (size_t pos = 0; pos < readSize; ) {
				// Which block is being used.
				const size_t blockIndex = (offset + pos) / BLOCK_SIZE;
				
				// Offset within the block.
				const size_t blockOffset = (offset + pos) % BLOCK_SIZE;
				
				const auto& block = cache_.getReadBlock(BlockPath::Index(blockIndex));
				
				const size_t blockReadSize = std::min<size_t>(readSize - pos, BLOCK_SIZE - blockOffset);
				memcpy(&buffer[pos], block.data() + blockOffset, blockReadSize);
				pos += blockReadSize;
			}
			
			return readSize;
		}
		
		size_t Node::write(NodeOffset offset, const uint8_t* buffer, size_t bufferSize) {
			if (offset > size()) {
				throw std::runtime_error("Offset exceeds size.");
			}
			
			if (bufferSize == 0) return 0;
			
			hasChanged_ = true;
			
			const size_t newSize = offset + bufferSize;
			if (newSize > size()) {
				BlockWriter sizeWriter(cache_.getWriteBlock(BlockPath::Root()), NODE_SIZE_OFFSET);
				Binary::WriteUint64(sizeWriter, newSize);
				
				size_ = newSize;
			}
			
			const size_t writeSize = bufferSize;
			
			for (size_t pos = 0; pos < writeSize; ) {
				// Which block is being used.
				const size_t blockIndex = (offset + pos) / BLOCK_SIZE;
				
				// Offset within the block.
				const size_t blockOffset = (offset + pos) % BLOCK_SIZE;
				
				auto& block = cache_.getWriteBlock(BlockPath::Index(blockIndex));
				
				const size_t blockWriteSize = std::min<size_t>(writeSize - pos, FolderSync::BLOCK_SIZE - blockOffset);
				memcpy(block.data() + blockOffset, &buffer[pos], blockWriteSize);
				pos += blockWriteSize;
			}
			
			return writeSize;
		}
		
	}
	
}

