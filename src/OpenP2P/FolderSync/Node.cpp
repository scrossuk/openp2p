#include <stdint.h>
#include <string.h>

#include <array>
#include <stdexcept>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/BlockReader.hpp>
#include <OpenP2P/FolderSync/BlockWriter.hpp>
#include <OpenP2P/FolderSync/Constants.hpp>
#include <OpenP2P/FolderSync/Database.hpp>
#include <OpenP2P/FolderSync/Node.hpp>

#include <OpenP2P/Stream/BinaryStream.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		namespace {
			
			NodeSize getSize(const Block& block) {
				BlockReader reader(block, NODE_SIZE_OFFSET);
				return Binary::ReadUint64(reader);
			}
			
			void setSize(Block& block, NodeSize size) {
				BlockWriter writer(block, NODE_SIZE_OFFSET);
				return Binary::WriteUint64(writer, size);
			}
			
			NodeType getType(const Block& block) {
				BlockReader reader(block, NODE_TYPE_OFFSET);
				const uint8_t typeVal = Binary::ReadUint8(reader);
				if (typeVal == 0) {
					return TYPE_DIRECTORY;
				} else {
					return TYPE_FILE;
				}
			}
			
			void setType(Block& block, NodeType type) {
				BlockWriter writer(block, NODE_TYPE_OFFSET);
				return Binary::WriteUint8(writer, type == TYPE_DIRECTORY ? 0 : 1);
			}
			
			size_t blockCount(NodeSize size) {
				return (size + (BLOCK_SIZE - 1)) / BLOCK_SIZE;
			}
			
			size_t blockIdPosition(size_t blockOffset) {
				return NODE_BLOCK_ID_OFFSET + BLOCK_ID_SIZE * blockOffset;
			}
			
			BlockId getBlockId(const Block& block, size_t blockOffset) {
				if (blockOffset >= NODE_MAX_BLOCKS) {
					throw std::runtime_error("Block offset exceeds maximum value.");
				}
				
				BlockReader reader(block, blockIdPosition(blockOffset));
				return BlockId::FromReader(reader);
			}
			
			void setBlockId(Block& block, size_t blockOffset, const BlockId& blockId) {
				if (blockOffset >= NODE_MAX_BLOCKS) {
					throw std::runtime_error("Block offset exceeds maximum value.");
				}
				
				BlockWriter writer(block, blockIdPosition(blockOffset));
				blockId.writeTo(writer);
			}
			
		}
		
		BlockId CreateEmptyNode(Database& database, NodeType type) {
			auto emptyBlock = Block::Zero();
			setSize(emptyBlock, 0);
			setType(emptyBlock, type);
			
			const auto emptyBlockId = BlockId::Generate(emptyBlock);
			database.storeBlock(emptyBlockId, std::move(emptyBlock));
			
			return emptyBlockId;
		}
		
		Node::Node(Database& database, const BlockId& initialBlockId)
			: blockStore_(database, initialBlockId),
			cache_(blockStore_) { }
		
		Node::~Node() {
			flush();
		}
		
		BlockId Node::blockId() const {
			return blockStore_.rootId();
		}
		
		NodeSize Node::size() const {
			return getSize(cache_.getReadBlock(BlockPath::Root()));
		}
		
		NodeType Node::type() const {
			return getType(cache_.getReadBlock(BlockPath::Root()));
		}
		
		void Node::flush() {
			cache_.flush();
		}
		
		void Node::resize(NodeSize newSize) {
			// Flush all data blocks, so that IDs of
			// deleted blocks aren't updated incorrectly
			// at some later time.
			cache_.flush();
			
			const size_t oldBlockCount = blockCount(size());
			const size_t newBlockCount = blockCount(newSize);
			
			// Set the size field.
			setSize(cache_.getWriteBlock(BlockPath::Root()), newSize);
			
			// Replace block IDs with zero for deleted blocks.
			if (oldBlockCount > newBlockCount) {
				const auto ZERO_ID = BlockId::Zero();
				
				for (size_t i = newBlockCount; i < oldBlockCount; i++) {
					const auto path = BlockPath::Index(i);
					auto& parentBlock = cache_.getWriteBlock(path.parent());
					setBlockId(parentBlock, path.back(), ZERO_ID);
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
			
			const size_t newSize = offset + bufferSize;
			if (newSize > size()) {
				setSize(cache_.getWriteBlock(BlockPath::Root()), newSize);
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

