#ifndef OPENP2P_FOLDERSYNC_BLOCKID_HPP
#define OPENP2P_FOLDERSYNC_BLOCKID_HPP

#include <stdint.h>

#include <array>
#include <functional>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/Constants.hpp>

#include <OpenP2P/Stream/BinaryStream.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		class BlockId {
			public:
				BlockId();
				
				static BlockId Zero();
				static BlockId FromReader(BlockingReader& reader);
				
				static BlockId Generate(const Block& data);
				
				void writeTo(BlockingWriter& writer) const;
				
				bool operator==(const BlockId&) const;
				bool operator<(const BlockId&) const;
				
				std::size_t hash() const;
				
				std::string hexString() const;
				
			private:
				std::array<uint8_t, BLOCK_ID_SIZE> data_;
			
		};
		
	}
	
}

namespace std {
	
	template<>
	struct hash<OpenP2P::FolderSync::BlockId> {
		std::size_t operator()(const OpenP2P::FolderSync::BlockId& blockId) const {
			return blockId.hash();
		}
	};
	
}

#endif
