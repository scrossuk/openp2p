#ifndef P2P_ROOTNETWORK_PARAMETERS_HPP
#define P2P_ROOTNETWORK_PARAMETERS_HPP

#include <stddef.h>

#include <p2p/Root/Root_api.hpp>

namespace p2p {

	namespace Root {
	
		constexpr size_t NETWORK_ID_SIZE_BITS = 64;
		constexpr size_t NETWORK_ID_SIZE_BYTES = NETWORK_ID_SIZE_BITS / 8;
		
		constexpr size_t NODE_ID_SIZE_BITS = 256;
		constexpr size_t NODE_ID_SIZE_BYTES = NODE_ID_SIZE_BITS / 8;
		
		constexpr size_t HEADER_SIZE_BYTES = 48;
		constexpr size_t SIGNATURE_SIZE_BYTES = 64;
		constexpr size_t PUBLIC_KEY_SIZE_BYTES = 32;
		constexpr size_t IDENTITY_SIZE_BYTES = SIGNATURE_SIZE_BYTES + PUBLIC_KEY_SIZE_BYTES;
		
		// Amount of concurrency for lookups.
		constexpr size_t DHT_NUM_PROBES = 3;
		
		// Maximum number of nodes in each bucket.
		constexpr size_t DHT_MAX_BUCKET_SIZE = 20;
		
		// Maximum number of nodes in a group.
		constexpr size_t DHT_MAX_GROUP_SIZE = 20; 
		
	}
	
}

#endif
