#ifndef P2P_FOLDERSYNC_NODEWRITER_HPP
#define P2P_FOLDERSYNC_NODEWRITER_HPP

#include <stdint.h>

#include <p2p/FolderSync/Node.hpp>

#include <p2p/Transport.hpp>

namespace p2p {
	
	namespace FolderSync {
		
		class NodeWriter: public BlockingWriter {
			public:
				NodeWriter(Node& node, NodeOffset initialPosition = 0);
				
				NodeOffset position() const;
				
				void seek(NodeOffset newPosition);
				
				void writeAll(const uint8_t* data, size_t size);
				
			private:
				Node& node_;
				NodeOffset position_;
				
		};
		
	}
	
}

#endif
