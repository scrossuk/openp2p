#include <stdint.h>

#include <array>
#include <stdexcept>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/Database.hpp>
#include <OpenP2P/FolderSync/Node.hpp>
#include <OpenP2P/Stream/BinaryStream.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		template <size_t SIZE>
		class ArrayReader: public BlockingReader {
			public:
				ArrayReader(const std::array<uint8_t, SIZE>& array, size_t position = 0)
					: array_(array), position_(position) { }
				
				void readAll(uint8_t* data, size_t size) {
					for (size_t i = 0; i < size; i++) {
						data[i] = array_.at(position_ + i);
					}
					position_ += size;
				}
				
			private:
				const std::array<uint8_t, SIZE>& array_;
				size_t position_;
			
		};
		
		Node::Node(Database& database, const BlockId& blockId)
			: database_(database), nodeBlockId_(blockId),
			nodeBlock_(database.loadBlock(blockId)) { }
		
		Node::~Node() { }
		
		NodeSize Node::size() const {
			ArrayReader<BLOCK_SIZE> reader(nodeBlock_);
			return  Binary::ReadUint64(reader);
		}
		
		BlockId Node::getBlockId(size_t blockOffset) const {
			if (blockOffset >= MAX_NODE_BLOCKS) {
				throw std::runtime_error("Block offset exceeds maximum value.");
			}
			
			if (blockOffset >= blockCount()) {
				throw std::runtime_error("Block offset exceeds size.");
			}
			
			const size_t position = METADATA_SIZE + BLOCK_ID_SIZE * blockOffset;
			std::array<uint8_t, BLOCK_ID_SIZE> blockIdArray;
			for (size_t i = 0; i < BLOCK_ID_SIZE; i++) {
				blockIdArray.at(i) = nodeBlock_.at(position + i);
			}
			return BlockId(blockIdArray);
		}
		
	}
	
}

