#ifndef OPENP2P_ROOTNETWORK_DHTRPCSERVICE_HPP
#define OPENP2P_ROOTNETWORK_DHTRPCSERVICE_HPP

#include <stdint.h>

#include <queue>

#include <OpenP2P/Socket.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Message.hpp>
#include <OpenP2P/RootNetwork/NetworkId.hpp>
#include <OpenP2P/RootNetwork/RoutineIdGenerator.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
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
