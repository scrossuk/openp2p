#include <stdint.h>
#include <string.h>

#include <array>
#include <stdexcept>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/Database.hpp>
#include <OpenP2P/FolderSync/Node.hpp>
#include <OpenP2P/Stream/BinaryStream.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		// Offset in the block where size is stored.
		constexpr size_t SIZE_OFFSET = 0;
		
		// Offset in the block where type is stored.
		constexpr size_t TYPE_OFFSET = 8;
		
		// Offset in the block where IDs are stored.
		constexpr size_t BLOCK_ID_OFFSET = 16;
		
		// Maximum node size in blocks.
		constexpr size_t MAX_NODE_BLOCKS = (BLOCK_SIZE - BLOCK_ID_OFFSET) / BLOCK_ID_SIZE;
		
		namespace {
			
			class BlockReader: public BlockingReader {
				public:
					BlockReader(const Block& block, size_t position = 0)
						: block_(block), position_(position) { }
					
					void readAll(uint8_t* data, size_t size) {
						for (size_t i = 0; i < size; i++) {
							data[i] = block_.at(position_ + i);
						}
						position_ += size;
					}
					
				private:
					const Block& block_;
					size_t position_;
				
			};
			
			class BlockWriter: public BlockingWriter {
				public:
					BlockWriter(Block& block, size_t position = 0)
						: block_(block), position_(position) { }
					
					void writeAll(const uint8_t* data, size_t size) {
						for (size_t i = 0; i < size; i++) {
							block_.at(position_ + i) = data[i];
						}
						position_ += size;
					}
					
				private:
					Block& block_;
					size_t position_;
				
			};
			
			NodeSize getSize(const Block& block) {
				BlockReader reader(block, SIZE_OFFSET);
				return Binary::ReadUint64(reader);
			}
			
			void setSize(Block& block, NodeSize size) {
				BlockWriter writer(block, SIZE_OFFSET);
				return Binary::WriteUint64(writer, size);
			}
			
			NodeType getType(const Block& block) {
				BlockReader reader(block, TYPE_OFFSET);
				const uint8_t typeVal = Binary::ReadUint8(reader);
				if (typeVal == 0) {
					return TYPE_DIRECTORY;
				} else {
					return TYPE_FILE;
				}
			}
			
			void setType(Block& block, NodeType type) {
				BlockWriter writer(block, TYPE_OFFSET);
				return Binary::WriteUint8(writer, type == TYPE_DIRECTORY ? 0 : 1);
			}
			
			size_t blockCount(NodeSize size) {
				return (size + (BLOCK_SIZE - 1)) / BLOCK_SIZE;
			}
			
			size_t blockIdPosition(size_t blockOffset) {
				return BLOCK_ID_OFFSET + BLOCK_ID_SIZE * blockOffset;
			}
			
			BlockId getBlockId(const Block& block, size_t blockOffset) {
				if (blockOffset >= MAX_NODE_BLOCKS) {
					throw std::runtime_error("Block offset exceeds maximum value.");
				}
				
				BlockReader reader(block, blockIdPosition(blockOffset));
				return BlockId::FromReader(reader);
			}
			
			void setBlockId(Block& block, size_t blockOffset, const BlockId& blockId) {
				if (blockOffset >= MAX_NODE_BLOCKS) {
					throw std::runtime_error("Block offset exceeds maximum value.");
				}
				
				BlockWriter writer(block, blockIdPosition(blockOffset));
				blockId.writeTo(writer);
			}
			
			Block zeroBlock() {
				Block emptyBlock;
				emptyBlock.fill(0x00);
				return emptyBlock;
			}
			
		}
		
		Node::Node(Database& database, const BlockId& initialBlockId)
			: database_(database), nodeBlockId_(initialBlockId),
			nodeBlock_(database.loadBlock(initialBlockId)),
			size_(getSize(nodeBlock_)) {
				database_.storeBlock(BlockId::ZeroBlockId(), zeroBlock());
			}
		
		Node Node::Empty(Database& database, NodeType type) {
			Block emptyBlock;
			emptyBlock.fill(0x00);
			setSize(emptyBlock, 0);
			setType(emptyBlock, type);
			
			const auto zeroBlockId = BlockId::ZeroBlockId();
			for (size_t i = 0; i < MAX_NODE_BLOCKS; i++) {
				setBlockId(emptyBlock, i, zeroBlockId);
			}
			
			const auto emptyBlockId = BlockId::Generate(emptyBlock);
			database.storeBlock(emptyBlockId, emptyBlock);
			
			return Node(database, emptyBlockId);
		}
		
		Node::~Node() {
			sync();
		}
		
		BlockId Node::blockId() const {
			return nodeBlockId_;
		}
		
		NodeSize Node::size() const {
			return size_;
		}
		
		NodeType Node::type() const {
			return getType(nodeBlock_);
		}
		
		void Node::sync() {
			database_.storeBlock(blockId(), nodeBlock_);
		}
		
		void Node::resize(NodeSize newSize) {
			const size_t oldBlockCount = blockCount(size());
			const size_t newBlockCount = blockCount(newSize);
			
			// Set the size field.
			setSize(nodeBlock_, newSize);
			size_ = newSize;
			
			// Replace block IDs with the ZeroBlock ID for deleted blocks.
			if (oldBlockCount > newBlockCount) {
				const auto zeroBlockId = BlockId::ZeroBlockId();
				for (size_t i = newBlockCount; i < oldBlockCount; i++) {
					setBlockId(nodeBlock_, i, zeroBlockId);
				}
			}
		}
		
		size_t Node::read(NodeOffset offset, uint8_t* buffer, size_t bufferSize) const {
			if (offset > size()) {
				throw std::runtime_error("Offset exceeds size.");
			}
			
			const size_t readSize = std::min<size_t>(bufferSize, size() - offset);
			
			for (size_t pos = 0; pos < readSize; ) {
				const size_t blockIndex = (offset + pos) / BLOCK_SIZE;
				const size_t blockPosition = (offset + pos) % BLOCK_SIZE;
				
				const auto block = database_.loadBlock(getBlockId(nodeBlock_, blockIndex));
				
				const size_t blockReadSize = std::min<size_t>(readSize - pos, BLOCK_SIZE - blockPosition);
				memcpy(&buffer[pos], &block[blockPosition], blockReadSize);
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
				setSize(nodeBlock_, newSize);
				size_ = newSize;
			}
			
			const size_t writeSize = bufferSize;
			
			for (size_t pos = 0; pos < writeSize; ) {
				const size_t blockIndex = (offset + pos) / FolderSync::BLOCK_SIZE;
				const size_t blockPosition = (offset + pos) % FolderSync::BLOCK_SIZE;
				
				auto dataBlock = database_.loadBlock(getBlockId(nodeBlock_, blockIndex));
				
				const size_t blockWriteSize = std::min<size_t>(writeSize - pos, FolderSync::BLOCK_SIZE - blockPosition);
				memcpy(&dataBlock[blockPosition], &buffer[pos], blockWriteSize);
				
				const auto dataBlockId = BlockId::Generate(dataBlock);
				
				database_.storeBlock(dataBlockId, dataBlock);
				setBlockId(nodeBlock_, blockIndex, dataBlockId);
				
				pos += blockWriteSize;
			}
			
			nodeBlockId_ = BlockId::Generate(nodeBlock_);
			
			return writeSize;
		}
		
	}
	
}

