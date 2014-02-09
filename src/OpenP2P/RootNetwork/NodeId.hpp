#ifndef OPENP2P_ROOTNETWORK_NODEID_HPP
#define OPENP2P_ROOTNETWORK_NODEID_HPP

#include <array>

#include <OpenP2P/RootNetwork/Parameters.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		typedef std::array<uint8_t, NODE_ID_SIZE_BYTES> NodeId;
		
	}
	
}

#endif
