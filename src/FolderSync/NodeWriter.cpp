#include <stdint.h>
#include <string.h>

#include <array>
#include <stdexcept>

#include <p2p/FolderSync/Node.hpp>
#include <p2p/FolderSync/NodeWriter.hpp>
#include <p2p/Transport.hpp>

namespace p2p {
	
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

