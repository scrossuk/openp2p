#ifndef P2P_ROOTNETWORK_CORE_RPCSERVICE_HPP
#define P2P_ROOTNETWORK_CORE_RPCSERVICE_HPP

#include <stdint.h>

#include <queue>

#include <p2p/Socket.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/RoutineIdGenerator.hpp>

namespace p2p {

	namespace Root {
	
		namespace Core {
		
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
	
}

#endif
