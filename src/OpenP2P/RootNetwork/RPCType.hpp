#ifndef OPENP2P_ROOTNETWORK_RPCTYPE_HPP
#define OPENP2P_ROOTNETWORK_RPCTYPE_HPP

#include <OpenP2P/RootNetwork/Id.hpp>

namespace OpenP2P{

	namespace RootNetwork{
	
		enum RPCType{
			RPC_FINDNODE = 0,
			RPC_PING = 1,
			RPC_GETSUBSCRIBERS = 2,
			RPC_SUBSCRIBE = 3
		};
	
	}
	
}

#endif
