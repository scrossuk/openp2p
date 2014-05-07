#ifndef P2P_ROOTNETWORK_CORE_RPCCLIENT_HPP
#define P2P_ROOTNETWORK_CORE_RPCCLIENT_HPP

#include <stdint.h>

#include <p2p/Event/UnionGenerator.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/MessageResender.hpp>
#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/NodePair.hpp>
#include <p2p/Root/RoutineIdGenerator.hpp>

#include <p2p/RPC/Host.hpp>

#include <p2p/Transport/Socket.hpp>

namespace p2p {

	namespace Root {
	
		namespace Core {
		
			class RPCClient {
				public:
					RPCClient(Socket<NodePair, Message>& socket, RoutineIdGenerator& routineIdGenerator, double timeoutMilliseconds = 500.0);
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
					MessageResender<NodePair> resender_;
					Event::UnionGenerator unionGenerator_;
					RoutineIdGenerator& routineIdGenerator_;
					RPC::Host<NodeId, RoutineId> identifyHost_;
					RPC::Host<Endpoint, RoutineId> pingHost_;
					RPC::Host<std::vector<NetworkId>, RoutineId> queryNetworksHost_;
					
			};
			
		}
		
	}
	
}

#endif
