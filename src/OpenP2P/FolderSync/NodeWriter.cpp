#include <stdint.h>
#include <string.h>

#include <array>
#include <stdexcept>

#include <OpenP2P/FolderSync/Node.hpp>
#include <OpenP2P/FolderSync/NodeWriter.hpp>
#include <OpenP2P/Stream/BinaryStream.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		NodeWriter::NodeWriter(Node& node, NodeOffset position)
			: node_(node), position_(0) {
				seek(position);
		}
		
		NodeOffset NodeWriter::position() const {
			return position_;
		}
		
		void NodeWriter::seek(NodeOffset position) {
			if (position > node_.size()) {
				throw std::runtime_error("Seek position exceeds node size.");
			}
			
			position_ = position;
		}
		
		void NodeWriter::writeAll(const uint8_t* data, size_t size) {
			node_.write(position_, data, size);
			position_ += size;
		}
		
	}
	
}

