#include <assert.h>
#include <stdint.h>
#include <string.h>

#include <array>
#include <stdexcept>

#include <p2p/FolderSync/Block.hpp>
#include <p2p/FolderSync/BlockId.hpp>
#include <p2p/FolderSync/Node.hpp>
#include <p2p/FolderSync/NodeReader.hpp>
#include <p2p/Transport.hpp>

namespace p2p {
	
	namespace FolderSync {
		
		NodeReader::NodeReader(const Node& node, NodeOffset initialPosition)
			: node_(node), position_(0) {
				seek(initialPosition);
		}
		
		NodeOffset NodeReader::position() const {
			return position_;
		}
		
		void NodeReader::seek(NodeOffset newPosition) {
			if (newPosition > node_.size()) {
				throw std::runtime_error("Seek position exceeds node size.");
			}
			
			position_ = newPosition;
		}
		
		void NodeReader::readAll(uint8_t* data, size_t size) {
			const size_t readSize = node_.read(position_, data, size);
			assert(readSize <= size);
			if (readSize < size) {
				throw std::runtime_error("Reached end of node data.");
			}
			position_ += size;
		}
		
	}
	
}

