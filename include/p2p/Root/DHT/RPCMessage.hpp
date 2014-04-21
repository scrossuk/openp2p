#ifndef P2P_ROOTNETWORK_DHT_RPCMESSAGE_HPP
#define P2P_ROOTNETWORK_DHT_RPCMESSAGE_HPP

#include <vector>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/NodeId.hpp>

namespace p2p {

	namespace Root {
	
		namespace DHT {
		
			struct NodeInfo {
				NodeId nodeId;
				std::vector<Endpoint> endpointList;
			};
			
			class RPCMessage {
				public:
					static RPCMessage GetNearestNodesRequest(const NodeId& targetId);
					static RPCMessage GetNearestNodesReply(const std::vector<NodeInfo>& nodeList);
					
					static RPCMessage SubscribeRequest(const NodeId& targetId, const std::vector<Endpoint>& myEndpoints);
					static RPCMessage SubscribeReply();
					
					static RPCMessage GetSubscribersRequest(const NodeId& targetId);
					static RPCMessage GetSubscribersReply(const std::vector<NodeInfo>& nodeList);
					
					enum Kind {
						GET_NEAREST_NODES = 0,
						SUBSCRIBE = 8,
						GET_SUBSCRIBERS = 9
					};
					
					Kind kind() const;
					State state() const;
					const NodeId& targetId() const;
					const std::vector<Endpoint>& endpointList() const;
					const std::vector<NodeInfo>& nodeList() const;
					
					Buffer toPayload() const;
					
					Message createMessage(uint32_t routine, const NodeId& sourceId, const NodeId& destinationId) const;
					
				private:
					RPCMessage(Kind kind, State state);
					
					Kind kind_;
					State state_;
					NodeId targetId_;
					std::vector<Endpoint> myEndpoints_;
					std::vector<NodeInfo> nodeList_;
					
					
			};
			
		}
		
	}
	
}

#endif
