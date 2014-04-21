#ifndef P2P_ROOTNETWORK_NETWORKID_HPP
#define P2P_ROOTNETWORK_NETWORKID_HPP

#include <stdint.h>

#include <array>
#include <functional>
#include <string>

#include <p2p/Stream/BinaryStream.hpp>

#include <p2p/Root/Parameters.hpp>

namespace p2p {

	namespace Root {
	
		class NetworkId {
			public:
				NetworkId();
				
				static NetworkId Zero();
				static NetworkId FromReader(BlockingReader& reader);
				
				static NetworkId Generate(const std::string& networkName);
				
				void writeTo(BlockingWriter& writer) const;
				
				bool operator==(const NetworkId&) const;
				bool operator<(const NetworkId&) const;
				
				std::size_t hash() const;
				
				std::string hexString() const;
				
			private:
				std::array<uint8_t, NETWORK_ID_SIZE_BYTES> data_;
			
		};
		
	}
	
}

namespace std {
	
	template<>
	struct hash<p2p::Root::NetworkId> {
		std::size_t operator()(const p2p::Root::NetworkId& id) const {
			return id.hash();
		}
	};
	
}

#endif
