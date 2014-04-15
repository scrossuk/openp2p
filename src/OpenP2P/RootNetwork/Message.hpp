#ifndef OPENP2P_ROOTNETWORK_MESSAGE_HPP
#define OPENP2P_ROOTNETWORK_MESSAGE_HPP

#include <stdint.h>

#include <boost/optional.hpp>

#include <OpenP2P/Buffer.hpp>

#include <OpenP2P/RootNetwork/NetworkId.hpp>
#include <OpenP2P/RootNetwork/NodeId.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>

namespace OpenP2P {

	namespace RootNetwork {
		
		/**
		 * \brief Message
		 * 
		 * Higher level representation of messages
		 * sent between nodes.
		 */
		struct Message {
			NodeId sourceId;
			NodeId destinationId;
			
			bool isError;
			
			boost::optional<NetworkId> subnetwork;
			uint8_t type;
			
			uint32_t routine;
			State routineState;
			
			Buffer payload;
			
			inline Message()
				: isError(false),
				type(0),
				routine(0),
				routineState(STATE_0) { }
		};
		
	}
	
}

#endif
