#ifndef OPENP2P_ROOTNETWORK_PARAMETERS_HPP
#define OPENP2P_ROOTNETWORK_PARAMETERS_HPP

#include <cstddef>

namespace OpenP2P{

	namespace RootNetwork{

		const std::size_t NumProbes = 3; //Amount of concurrency for lookups
		const std::size_t IdSize = 64; //64 bytes = 512 bits
		const std::size_t MaxBucketSize = 20; //Maximum number of nodes in each bucket
		const std::size_t MaxGroupSize = 20; //Maximum number of nodes in a group
		
	}
	
}

#endif
