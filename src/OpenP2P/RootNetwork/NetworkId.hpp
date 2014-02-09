#ifndef OPENP2P_ROOTNETWORK_NETWORKID_HPP
#define OPENP2P_ROOTNETWORK_NETWORKID_HPP

#include <stdint.h>

#include <array>

#include <OpenP2P/RootNetwork/Parameters.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		typedef std::array<uint8_t, NETWORK_ID_SIZE_BYTES> NetworkId;
		
	}
	
}

#endif
