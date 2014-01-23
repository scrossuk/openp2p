#ifndef OPENP2P_FOLDERSYNC_NODE_HPP
#define OPENP2P_FOLDERSYNC_NODE_HPP

#include <vector>

#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/Database.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		// Size of the metadata stored in the node.
		constexpr size_t METADATA_SIZE = 16;
		
		// Maximum node (i.e. file) size in blocks.
		constexpr size_t MAX_NODE_BLOCKS = (BLOCK_SIZE - METADATA_SIZE) / BLOCK_ID_SIZE;
		
		typedef uint64_t NodeSize;
		
		class Node {
			public:
				Node(Database& database, const BlockId& blockId);
				~Node();
				
				/**
				 * \brief Get node size (in bytes).
				 */
				NodeSize size() const;
				
				inline size_t blockCount() const {
					return size() / BLOCK_SIZE;
				}
				
				/**
				 * \brief Get ID of a block with the given offset.
				 * 
				 * NOTE: Will throw if blockOffset exceeds 
				 */
				BlockId getBlockId(size_t blockOffset) const;
				
			private:
				Database& database_;
				BlockId nodeBlockId_;
				Block nodeBlock_;
			
		};
		
	}
	
}

#endif
