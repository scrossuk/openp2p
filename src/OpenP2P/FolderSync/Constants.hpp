#ifndef OPENP2P_FOLDERSYNC_CONSTANTS_HPP
#define OPENP2P_FOLDERSYNC_CONSTANTS_HPP

#include <stddef.h>
#include <stdint.h>

namespace OpenP2P {
	
	namespace FolderSync {
		
		// ------------- Configurable values.
		
		// Size of a block.
		constexpr size_t BLOCK_SIZE = 65536;
		
		// Size of a block ID.
		constexpr size_t BLOCK_ID_SIZE = 16;
		
		// Offset in a node block where size is stored.
		constexpr size_t NODE_SIZE_OFFSET = 0;
		
		// Offset in a node block where type is stored.
		constexpr size_t NODE_TYPE_OFFSET = 8;
		
		// Offset in the block where IDs are stored.
		constexpr size_t NODE_BLOCK_ID_OFFSET = 16;
		
		// Maximum blocks held by a block cache.
		constexpr size_t CACHE_MAX_BLOCKS = 8;
		
		// Number of indirects in a node's root block.
		constexpr size_t INDIRECT_COUNT = 3;
		
		// ------------- Calculated values.
		
		// Maximum node block path size.
		constexpr size_t NODE_MAX_BLOCK_PATH = INDIRECT_COUNT + 1;
		
		// Maximum references in node's root block.
		constexpr size_t NODE_ROOT_MAX_REFS = ((BLOCK_SIZE - NODE_BLOCK_ID_OFFSET) / BLOCK_ID_SIZE);
		
		// Maximum direct references in node's root block.
		constexpr size_t NODE_ROOT_MAX_DIRECT_REFS = NODE_ROOT_MAX_REFS - INDIRECT_COUNT;
		
		// Maximum references in node's indirect blocks.
		constexpr size_t NODE_MAX_INDIRECT_REFS = (BLOCK_SIZE / BLOCK_ID_SIZE);
		
		// Index of the single indirect in node's root block.
		constexpr size_t NODE_SINGLE_INDIRECT_INDEX = NODE_ROOT_MAX_DIRECT_REFS;
		
		// Index of the double indirect in node's root block.
		constexpr size_t NODE_DOUBLE_INDIRECT_INDEX = NODE_ROOT_MAX_DIRECT_REFS + 1;
		
		// Index of the triple indirect in node's root block.
		constexpr size_t NODE_TRIPLE_INDIRECT_INDEX = NODE_ROOT_MAX_DIRECT_REFS + 2;
		
		// Integer power.
		constexpr size_t INT_POW(size_t value, size_t power) {
			return power > 0 ? value * INT_POW(value, power - 1) : 1;
		}
		
		// Integer power sum (i.e. INT_POW_SUM(n, 2, c) = n^2 + n^1 + c).
		constexpr size_t INT_POW_SUM(size_t value, size_t power, size_t constant) {
			return power > 0 ? INT_POW(value, power) + INT_POW_SUM(value, power - 1, constant) :
				constant;
		}
		
		// Calculate blocks that can be referenced by an indirect level.
		constexpr size_t BLOCK_COUNT(size_t indirectLevel) {
			return INT_POW_SUM(NODE_MAX_INDIRECT_REFS, indirectLevel, NODE_ROOT_MAX_DIRECT_REFS);
		}
		
		// Maximum node size in blocks.
		constexpr size_t NODE_MAX_BLOCKS = BLOCK_COUNT(INDIRECT_COUNT);
		
		// Maximum node size in bytes.
		constexpr size_t NODE_MAX_BYTES = NODE_MAX_BLOCKS * BLOCK_SIZE;
		
	}
	
}

#endif
