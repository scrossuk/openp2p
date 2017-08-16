#ifndef P2P_ROOTNETWORK_MESSAGE_HPP
#define P2P_ROOTNETWORK_MESSAGE_HPP

#include <stdint.h>

#include <boost/optional.hpp>

#include <p2p/Root/Root_api.hpp>
#include <p2p/Transport.hpp>

#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/Packet.hpp>

namespace p2p {

	namespace Root {
		
		/**
		 * \brief Message
		 * 
		 * Higher level representation of messages
		 * sent between nodes.
		 */
		struct Message {
			boost::optional<NetworkId> subnetwork;
			uint8_t type;
			
			uint32_t routine;
			State routineState;
			
			Buffer payload;
			
			inline Message()
				: subnetwork(boost::none),
				type(0),
				routine(0),
				routineState(STATE_0) { }
		};
		
	}
	
}

#endif
