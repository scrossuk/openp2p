#ifndef P2P_ROOTNETWORK_DHT_RPCCLIENT_HPP
#define P2P_ROOTNETWORK_DHT_RPCCLIENT_HPP

#include <stdint.h>

#include <queue>

#include <p2p/Event/Event_api.hpp>
#include <p2p/Event/UnionGenerator.hpp>

#include <p2p/Transport/Socket.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/MessageResender.hpp>
#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/NodeInfo.hpp>
#include <p2p/Root/RoutineIdGenerator.hpp>

#include <p2p/RPC/Host.hpp>
#include <p2p/RPC/Operation.hpp>

namespace p2p {

	namespace Root {
	
		namespace DHT {
		
			struct Empty {};
			
			class ROOT_OPENP2P_API RPCClient {
				public:
					RPCClient(Socket<NodeId, Message>& socket, RoutineIdGenerator& routineIdGenerator, double timeoutMilliseconds = 500.0);
					~RPCClient();
					
					Event::Source eventSource() const;
					
					bool processResponse();
					
					RPC::Operation<std::vector<NodeInfo>> getNearestNodes(const NodeId& destId, const NodeId& targetId);
					
					RPC::Operation<Empty> subscribe(const NodeId& destId, const NodeId& targetId, const std::vector<Endpoint>& myEndpoints);
					
					RPC::Operation<std::vector<NodeInfo>> getSubscribers(const NodeId& destId, const NodeId& targetId);
					
				private:
					// Non-copyable.
					RPCClient(const RPCClient&) = delete;
					RPCClient& operator=(RPCClient) = delete;
					
					Socket<NodeId, Message>& socket_;
					MessageResender<NodeId> resender_;
					Event::UnionGenerator unionGenerator_;
					RoutineIdGenerator& routineIdGenerator_;
					RPC::Host<std::vector<NodeInfo>, RoutineId> getNearestHost_;
					RPC::Host<Empty, RoutineId> subscribeHost_;
					RPC::Host<std::vector<NodeInfo>, RoutineId> getSubscribersHost_;
					
			};
			
		}
		
	}
	
}

#endif
