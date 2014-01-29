#include <assert.h>
#include <stdint.h>
#include <string.h>

#include <array>
#include <stdexcept>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/Node.hpp>
#include <OpenP2P/FolderSync/NodeReader.hpp>
#include <OpenP2P/Stream/BinaryStream.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		NodeReader::NodeReader(Node& node, NodeOffset position)
			: node_(node), position_(0) {
				seek(position);
		}
		
		NodeOffset NodeReader::position() const {
			return position_;
		}
		
		void NodeReader::seek(NodeOffset position) {
			if (position > node_.size()) {
				throw std::runtime_error("Seek position exceeds node size.");
			}
			
			position_ = position;
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

