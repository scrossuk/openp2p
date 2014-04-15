#ifndef OPENP2P_ROOTNETWORK_RPCSERVICE_HPP
#define OPENP2P_ROOTNETWORK_RPCSERVICE_HPP

#include <stdint.h>

#include <queue>

#include <OpenP2P/Socket.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Message.hpp>
#include <OpenP2P/RootNetwork/NetworkId.hpp>
#include <OpenP2P/RootNetwork/RoutineIdGenerator.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		class RPCService {
			public:
				RPCService(Socket<Endpoint, Message>& socket, RoutineIdGenerator& routineIdGenerator);
				~RPCService();
				
				void addNetwork(const std::string& networkName);
				
				NodeId identifyEndpoint(const Endpoint& endpoint);
				
				Endpoint pingNode(const Endpoint& endpoint, const NodeId& nodeId);
				
				std::vector<NetworkId> queryNetworks(const Endpoint& endpoint, const NodeId& nodeId);
				
				void processRequests();
				
			private:
				// Non-copyable.
				RPCService(const RPCService&) = delete;
				RPCService& operator=(RPCService) = delete;
				
				Socket<Endpoint, Message>& socket_;
				RoutineIdGenerator& routineIdGenerator_;
				std::vector<NetworkId> networks_;
				
		};
		
	}
	
}

#endif
