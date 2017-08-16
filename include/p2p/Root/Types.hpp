#ifndef P2P_ROOTNETWORK_TYPES_HPP
#define P2P_ROOTNETWORK_TYPES_HPP

#include <stdint.h>

#include <p2p/Root/Root_api.hpp>

namespace p2p {

	namespace Root {
		
		enum Version {
			VERSION_INVALID = 0,
			VERSION_1 = 1 // Currently only one version.
		};
		
		enum State {
			STATE_0 = 0,
			STATE_1 = 1,
			STATE_2 = 2,
			STATE_3 = 3
		};
		
		using MessageType = uint8_t;
		
		using PacketLength = uint16_t;
		
		using RoutineId = uint32_t;
		
		using MessageCounter = uint64_t;
		
	}
	
}

#endif
