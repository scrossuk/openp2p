#ifndef OPENP2P_FOLDERSYNC_NODEWRITER_HPP
#define OPENP2P_FOLDERSYNC_NODEWRITER_HPP

#include <stdint.h>

#include <OpenP2P/FolderSync/Node.hpp>

#include <OpenP2P/Stream/BinaryStream.hpp>

namespace OpenP2P {
	
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
