#ifndef OPENP2P_ROOTNETWORK_NODEID_HPP
#define OPENP2P_ROOTNETWORK_NODEID_HPP

#include <stdint.h>

#include <array>
#include <functional>

#include <OpenP2P/Stream/BinaryStream.hpp>

#include <OpenP2P/RootNetwork/Key.hpp>
#include <OpenP2P/RootNetwork/Parameters.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		class NodeId {
			public:
				static constexpr size_t SIZE_IN_BITS = NODE_ID_SIZE_BITS;
				
				NodeId();
				
				static NodeId Zero();
				static NodeId Max();
				
				static NodeId FromReader(BlockingReader& reader);
				
				static NodeId Generate(const PublicKey& publicKey);
				
				void writeTo(BlockingWriter& writer) const;
				
				bool bitAt(size_t index) const;
				
				bool operator==(const NodeId&) const;
				bool operator!=(const NodeId&) const;
				bool operator<(const NodeId&) const;
				
				NodeId operator^(const NodeId&) const;
				NodeId operator~() const;
				
				std::size_t hash() const;
				
				std::string hexString() const;
				
			private:
				std::array<uint8_t, NODE_ID_SIZE_BYTES> data_;
			
		};
		
	}
	
}

namespace std {
	
	template<>
	struct hash<OpenP2P::RootNetwork::NodeId> {
		std::size_t operator()(const OpenP2P::RootNetwork::NodeId& id) const {
			return id.hash();
		}
	};
	
}

#endif
