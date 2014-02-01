#include <assert.h>
#include <stdint.h>

#include <stdexcept>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/BlockReader.hpp>
#include <OpenP2P/FolderSync/BlockWriter.hpp>
#include <OpenP2P/FolderSync/Constants.hpp>
#include <OpenP2P/FolderSync/NodeBlockStore.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		namespace {
			
			size_t blockIdPosition(size_t blockPosition) {
				return NODE_BLOCK_ID_OFFSET + BLOCK_ID_SIZE * blockPosition;
			}
			
		}
		
		NodeBlockStore::NodeBlockStore(Database& database, Block& nodeBlock)
			: database_(database), nodeBlock_(nodeBlock) { }
		
		NodeBlockStore::~NodeBlockStore() { }
		
		Block NodeBlockStore::getBlock(size_t position) const {
			if (position >= NODE_MAX_BLOCKS) {
				throw std::runtime_error("Block position exceeds maximum value.");
			}
			
			BlockReader reader(nodeBlock_, blockIdPosition(position));
			const auto blockId = BlockId::FromReader(reader);
			
			return database_.loadBlock(blockId);
		}
		
		void NodeBlockStore::setBlock(size_t position, Block block) {
			if (position >= NODE_MAX_BLOCKS) {
				throw std::runtime_error("Block position exceeds maximum value.");
			}
			
			const auto blockId = BlockId::Generate(block);
			database_.storeBlock(blockId, std::move(block));
			
			BlockWriter writer(nodeBlock_, blockIdPosition(position));
			blockId.writeTo(writer);
		}
		
		void NodeBlockStore::unsetBlock(size_t position) {
			if (position >= NODE_MAX_BLOCKS) {
				throw std::runtime_error("Block position exceeds maximum value.");
			}
			
			BlockWriter writer(nodeBlock_, blockIdPosition(position));
			BlockId::Zero().writeTo(writer);
		}
		
	}
	
}

