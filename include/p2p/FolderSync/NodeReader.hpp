#ifndef P2P_FOLDERSYNC_NODEREADER_HPP
#define P2P_FOLDERSYNC_NODEREADER_HPP

#include <stdint.h>

#include <p2p/FolderSync/Node.hpp>

#include <p2p/Stream/BinaryStream.hpp>

namespace p2p {
	
	namespace FolderSync {
		
		class NodeReader: public BlockingReader {
			public:
				NodeReader(const Node& node, NodeOffset initialPosition = 0);
				
				NodeOffset position() const;
				
				void seek(NodeOffset newPosition);
				
				void readAll(uint8_t* data, size_t size);
				
			private:
				const Node& node_;
				NodeOffset position_;
				
		};
		
	}
	
}

#endif
