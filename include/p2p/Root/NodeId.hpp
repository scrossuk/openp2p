#ifndef P2P_ROOTNETWORK_NODEID_HPP
#define P2P_ROOTNETWORK_NODEID_HPP

#include <stdint.h>

#include <array>
#include <functional>

#include <p2p/Root/Root_api.hpp>
#include <p2p/Root/Key.hpp>
#include <p2p/Root/Parameters.hpp>

#include <p2p/Transport.hpp>

namespace p2p {

	namespace Root {
	
		class ROOT_OPENP2P_API NodeId {
			public:
				static constexpr size_t SIZE_IN_BITS = NODE_ID_SIZE_BITS;
				
				NodeId();
				
				static NodeId Zero();
				static NodeId Max();
				
				static NodeId FromReader(BlockingReader& reader);
				
				static NodeId Generate(const PublicKey& publicKey);
				
				void writeTo(BlockingWriter& writer) const;
				
				bool bitAt(size_t index) const;
				
				uint8_t operator[](size_t index) const;
				uint8_t& operator[](size_t index);
				
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
	struct hash<p2p::Root::NodeId> {
		std::size_t operator()(const p2p::Root::NodeId& id) const {
			return id.hash();
		}
	};
	
}

#endif
