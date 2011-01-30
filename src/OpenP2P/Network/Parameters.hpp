#ifndef OPENP2P_NETWORK_PARAMETERS_HPP
#define OPENP2P_NETWORK_PARAMETERS_HPP

#include <cstddef>

namespace OpenP2P{

	namespace Network{

		const std::size_t NumProbes = 3; //Amount of concurrency for lookups
		const std::size_t IdSize = 64; //64 bytes = 512 bits
		const std::size_t BucketSize = 20; //Number of nodes in each bucket
		
	}
	
}

#endif
