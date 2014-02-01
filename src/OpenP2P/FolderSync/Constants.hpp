#ifndef OPENP2P_FOLDERSYNC_CONSTANTS_HPP
#define OPENP2P_FOLDERSYNC_CONSTANTS_HPP

#include <stddef.h>
#include <stdint.h>

namespace OpenP2P {
	
	namespace FolderSync {
		
		// Size of a block.
		constexpr size_t BLOCK_SIZE = 65536 * 4;
		
		// Size of a block ID.
		constexpr size_t BLOCK_ID_SIZE = 16;
		
		// Offset in a node block where size is stored.
		constexpr size_t NODE_SIZE_OFFSET = 0;
		
		// Offset in a node block where type is stored.
		constexpr size_t NODE_TYPE_OFFSET = 8;
		
		// Offset in the block where IDs are stored.
		constexpr size_t NODE_BLOCK_ID_OFFSET = 16;
		
		// Maximum node size in blocks.
		constexpr size_t NODE_MAX_BLOCKS = (BLOCK_SIZE - NODE_BLOCK_ID_OFFSET) / BLOCK_ID_SIZE;
		
		// Maximum node size in bytes.
		constexpr size_t NODE_MAX_BYTES = NODE_MAX_BLOCKS * BLOCK_SIZE;
		
	}
	
}

#endif
