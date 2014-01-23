#ifndef OPENP2P_FOLDERSYNC_BLOCKID_HPP
#define OPENP2P_FOLDERSYNC_BLOCKID_HPP

#include <stdint.h>

#include <array>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/Stream/BinaryStream.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		constexpr size_t BLOCK_ID_SIZE = 16;
		
		class BlockId {
			public:
				BlockId();
				
				static BlockId ZeroBlockId();
				static BlockId FromReader(BlockingReader& reader);
				
				static BlockId Generate(const Block& data);
				
				void writeTo(BlockingWriter& writer) const;
				
				bool operator==(const BlockId&) const;
				bool operator<(const BlockId&) const;
				
			private:
				std::array<uint8_t, BLOCK_ID_SIZE> data_;
			
		};
		
	}
	
}

#endif
