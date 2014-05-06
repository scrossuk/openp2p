#ifndef P2P_ROOTNETWORK_CORE_RPCCLIENT_HPP
#define P2P_ROOTNETWORK_CORE_RPCCLIENT_HPP

#include <stdint.h>

#include <p2p/Transport/Socket.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/NodePair.hpp>
#include <p2p/Root/RoutineIdGenerator.hpp>

#include <p2p/RPC/Host.hpp>

namespace p2p {

	namespace Root {
	
		namespace Core {
		
			class RPCClient {
				public:
					RPCClient(Socket<NodePair, Message>& socket, RoutineIdGenerator& routineIdGenerator);
					~RPCClient();
					
					Event::Source eventSource() const;
					
					bool processResponse();
					
					RPC::Operation<NodeId> identify(const Endpoint& endpoint);
					
					RPC::Operation<Endpoint> ping(const Endpoint& endpoint, const NodeId& nodeId);
					
					RPC::Operation<std::vector<NetworkId>> queryNetworks(const Endpoint& endpoint, const NodeId& nodeId);
					
				private:
					// Non-copyable.
					RPCClient(const RPCClient&) = delete;
					RPCClient& operator=(RPCClient) = delete;
					
					Socket<NodePair, Message>& socket_;
					RoutineIdGenerator& routineIdGenerator_;
					RPC::Host<NodeId, RoutineId> identifyHost_;
					RPC::Host<Endpoint, RoutineId> pingHost_;
					RPC::Host<std::vector<NetworkId>, RoutineId> queryNetworksHost_;
					
			};
			
		}
		
	}
	
}

#endif
