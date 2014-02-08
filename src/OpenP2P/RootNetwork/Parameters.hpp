#ifndef OPENP2P_ROOTNETWORK_PARAMETERS_HPP
#define OPENP2P_ROOTNETWORK_PARAMETERS_HPP

#include <stddef.h>

namespace OpenP2P {

	namespace RootNetwork {
	
		constexpr size_t NODE_ID_SIZE_BITS = 256;
		constexpr size_t NODE_ID_SIZE_BYTES = NODE_ID_SIZE_BITS / 8;
		
		// Amount of concurrency for lookups.
		constexpr size_t DHT_NUM_PROBES = 3;
		
		// Maximum number of nodes in each bucket.
		constexpr size_t DHT_MAX_BUCKET_SIZE = 20;
		
		// Maximum number of nodes in a group.
		constexpr size_t DHT_MAX_GROUP_SIZE = 20; 
		
	}
	
}

#endif
