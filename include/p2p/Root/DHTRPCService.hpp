#ifndef P2P_ROOTNETWORK_DHTRPCSERVICE_HPP
#define P2P_ROOTNETWORK_DHTRPCSERVICE_HPP

#include <stdint.h>

#include <queue>

#include <p2p/Socket.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/RoutineIdGenerator.hpp>

namespace p2p {

	namespace Root {
	
		class DHTRPCService {
			public:
				DHTRPCService(Socket<NodeId, Message>& socket, RoutineIdGenerator& routineIdGenerator);
				~DHTRPCService();
				
				void getNearestNodes(const NodeId& destId, const NodeId& targetId);
				
				void subscribe(const NodeId& destId, const NodeId& targetId, const std::vector<Endpoint>& myEndpoints);
				
				void getSubscribers(const NodeId& destId, const NodeId& targetId);
				
			private:
				// Non-copyable.
				DHTRPCService(const DHTRPCService&) = delete;
				DHTRPCService& operator=(DHTRPCService) = delete;
				
				Socket<NodeId, Message>& socket_;
				RoutineIdGenerator& routineIdGenerator_;
				
		};
		
	}
	
}

#endif
