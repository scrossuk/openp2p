#ifndef OPENP2P_FOLDERSYNC_BLOCK_HPP
#define OPENP2P_FOLDERSYNC_BLOCK_HPP

#include <stdint.h>

#include <array>
#include <cstddef>

namespace OpenP2P {
	
	namespace FolderSync {
		
		constexpr size_t BLOCK_SIZE = 4096;
		
		typedef std::array<uint8_t, BLOCK_SIZE> Block;
		
	}
	
}

#endif
