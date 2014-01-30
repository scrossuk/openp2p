#include <stdint.h>
#include <string.h>

#include <array>
#include <stdexcept>

#include <OpenP2P/FolderSync/Node.hpp>
#include <OpenP2P/FolderSync/NodeWriter.hpp>
#include <OpenP2P/Stream/BinaryStream.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		NodeWriter::NodeWriter(Node& node, NodeOffset initialPosition)
			: node_(node), position_(0) {
				seek(initialPosition);
		}
		
		NodeOffset NodeWriter::position() const {
			return position_;
		}
		
		void NodeWriter::seek(NodeOffset newPosition) {
			if (newPosition > node_.size()) {
				throw std::runtime_error("Seek position exceeds node size.");
			}
			
			position_ = newPosition;
		}
		
		void NodeWriter::writeAll(const uint8_t* data, size_t size) {
			node_.write(position_, data, size);
			position_ += size;
		}
		
	}
	
}

