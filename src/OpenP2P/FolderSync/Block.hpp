#ifndef OPENP2P_FOLDERSYNC_BLOCK_HPP
#define OPENP2P_FOLDERSYNC_BLOCK_HPP

#include <stdint.h>

#include <cstddef>
#include <vector>

namespace OpenP2P {
	
	namespace FolderSync {
		
		typedef std::vector<uint8_t> Block;
		
		constexpr size_t BLOCK_MAX_SIZE = 64 * 1024;
		
	}
	
}

#endif
