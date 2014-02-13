#ifndef OPENP2P_ROOTNETWORK_NETWORKID_HPP
#define OPENP2P_ROOTNETWORK_NETWORKID_HPP

#include <stdint.h>

#include <array>
#include <functional>
#include <string>

#include <OpenP2P/Stream/BinaryStream.hpp>

#include <OpenP2P/RootNetwork/Parameters.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
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
	struct hash<OpenP2P::RootNetwork::NetworkId> {
		std::size_t operator()(const OpenP2P::RootNetwork::NetworkId& id) const {
			return id.hash();
		}
	};
	
}

#endif
