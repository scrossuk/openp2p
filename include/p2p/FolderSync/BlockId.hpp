#ifndef P2P_FOLDERSYNC_BLOCKID_HPP
#define P2P_FOLDERSYNC_BLOCKID_HPP

#include <stdint.h>

#include <array>
#include <functional>

#include <p2p/FolderSync/Block.hpp>
#include <p2p/FolderSync/Constants.hpp>

#include <p2p/Transport.hpp>

namespace p2p {
	
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
	struct hash<p2p::FolderSync::BlockId> {
		std::size_t operator()(const p2p::FolderSync::BlockId& blockId) const {
			return blockId.hash();
		}
	};
	
}

#endif
