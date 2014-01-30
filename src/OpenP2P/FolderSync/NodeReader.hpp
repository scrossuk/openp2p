#ifndef OPENP2P_FOLDERSYNC_NODEREADER_HPP
#define OPENP2P_FOLDERSYNC_NODEREADER_HPP

#include <stdint.h>

#include <OpenP2P/FolderSync/Node.hpp>

#include <OpenP2P/Stream/BinaryStream.hpp>

namespace OpenP2P {
	
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
