#ifndef OPENP2P_ROOTNETWORK_PACKET_HPP
#define OPENP2P_ROOTNETWORK_PACKET_HPP

#include <stdint.h>

#include <OpenP2P/Buffer.hpp>

#include <OpenP2P/RootNetwork/NodeId.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		struct Packet {
			NodeId sourceId;
			NodeId destinationId;
			
			bool err;
			
			boost::optional<NetworkId> subnetwork;
			uint8_t type;
			
			uint32_t routine;
			State routineState;
			
			Buffer payload;
		};
		
	}
	
}

#endif
