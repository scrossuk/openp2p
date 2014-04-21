#ifndef P2P_ROOTNETWORK_DHT_RPCSERVICE_HPP
#define P2P_ROOTNETWORK_DHT_RPCSERVICE_HPP

#include <stdint.h>

#include <queue>

#include <p2p/Socket.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/RoutineIdGenerator.hpp>

namespace p2p {

	namespace Root {
	
		namespace DHT {
		
			class RPCService {
				public:
					RPCService(Socket<NodeId, Message>& socket, RoutineIdGenerator& routineIdGenerator);
					~RPCService();
					
					void getNearestNodes(const NodeId& destId, const NodeId& targetId);
					
					void subscribe(const NodeId& destId, const NodeId& targetId, const std::vector<Endpoint>& myEndpoints);
					
					void getSubscribers(const NodeId& destId, const NodeId& targetId);
					
				private:
					// Non-copyable.
					RPCService(const RPCService&) = delete;
					RPCService& operator=(RPCService) = delete;
					
					Socket<NodeId, Message>& socket_;
					RoutineIdGenerator& routineIdGenerator_;
					
			};
			
		}
		
	}
	
}

#endif
