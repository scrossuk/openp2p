#ifndef P2P_FOLDERSYNC_BLOCKPATH_HPP
#define P2P_FOLDERSYNC_BLOCKPATH_HPP

#include <stddef.h>

#include <array>
#include <functional>

#include <p2p/FolderSync/Constants.hpp>

namespace p2p {
	
	namespace FolderSync {
		
		class BlockPath {
			public:
				static BlockPath Root();
				
				static BlockPath Index(size_t blockIndex);
				
				bool isRoot() const;
				
				size_t size() const;
				
				size_t at(size_t index) const;
				
				size_t back() const;
				
				BlockPath parent() const;
				
				bool operator==(const BlockPath& path) const;
				
				std::size_t hash() const;
				
			private:
				BlockPath();
				BlockPath(uint8_t pSize, const std::array<uint16_t, NODE_MAX_BLOCK_PATH>& components);
				
				uint8_t size_;
				std::array<uint16_t, NODE_MAX_BLOCK_PATH> components_;
			
		};
		
	}
	
}

namespace std {
	
	template<>
	struct hash<p2p::FolderSync::BlockPath> {
		std::size_t operator()(const p2p::FolderSync::BlockPath& path) const {
			return path.hash();
		}
	};
	
}

#endif
