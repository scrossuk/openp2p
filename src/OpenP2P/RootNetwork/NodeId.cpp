#include <array>

#include <boost/functional/hash.hpp>

#include <OpenP2P/Stream/BinaryStream.hpp>

#include <OpenP2P/RootNetwork/NodeId.hpp>
#include <OpenP2P/RootNetwork/Parameters.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		NodeId::NodeId() {
			data_.fill(0x00);
		}
		
		NodeId NodeId::Zero() {
			return NodeId();
		}
		
		NodeId NodeId::FromReader(BlockingReader& reader) {
			NodeId id;
			reader.readAll(id.data_.data(), id.data_.size());
			return id;
		}
		
		NodeId NodeId::Generate(const PublicKey&) {
			// TODO
			return NodeId();
		}
		
		void NodeId::writeTo(BlockingWriter& writer) const {
			writer.writeAll(data_.data(), data_.size());
		}
		
		bool NodeId::operator==(const NodeId& other) const {
			return data_ == other.data_;
		}
		
		bool NodeId::operator<(const NodeId& other) const {
			return data_ < other.data_;
		}
		
		std::size_t NodeId::hash() const {
			return boost::hash_range(data_.begin(), data_.end());
		}
		
	}
	
}

