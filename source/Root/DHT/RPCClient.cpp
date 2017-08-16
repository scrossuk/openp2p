#include <stdio.h>

#include <queue>

#include <p2p/Transport/Socket.hpp>

#include <p2p/Event/Source.hpp>
#include <p2p/Event/Wait.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/PrivateIdentity.hpp>
#include <p2p/Root/PublicIdentity.hpp>
#include <p2p/Root/RoutineIdGenerator.hpp>

#include <p2p/Root/DHT/RPCClient.hpp>
#include <p2p/Root/DHT/RPCMessage.hpp>

#include <p2p/RPC/Host.hpp>
#include <p2p/RPC/Operation.hpp>

namespace p2p {

	namespace Root {
	
		namespace DHT {
		
			std::vector<NodeInfo> readNodeGroup(const Buffer& payload) {
				std::vector<NodeInfo> group;
				
				BufferIterator iterator(payload);
				BinaryIStream blockingReader(iterator);
				
				while (iterator.position() < payload.size()) {
					group.push_back(NodeInfo::Read(blockingReader));
				}
				
				return group;
			}
			
			RPCClient::RPCClient(Socket<NodeId, Message>& socket, RoutineIdGenerator& routineIdGenerator, double timeoutMilliseconds)
				: socket_(socket), resender_(socket_, timeoutMilliseconds),
				unionGenerator_({ socket_.eventSource(), resender_.eventSource() }),
				routineIdGenerator_(routineIdGenerator) { }
				
			RPCClient::~RPCClient() { }
			
			Event::Source RPCClient::eventSource() const {
				return unionGenerator_.eventSource();
			}
			
			bool RPCClient::processResponse() {
				resender_.processResends();
				
				NodeId senderId;
				Message message;
				const bool result = socket_.receive(senderId, message);
				
				if (!result) {
					return false;
				}
				
				if (message.routineState != STATE_1) {
					// Not a response.
					return false;
				}
				
				if (!message.subnetwork || *(message.subnetwork) != NetworkId::Generate("p2p.rootdht")) {
					// Response is not for this subnetwork.
					return false;
				}
				
				switch (message.type) {
					case RPCMessage::GET_NEAREST_NODES: {
						resender_.endRoutine(message.routine);
						getNearestHost_.completeOperation(message.routine, readNodeGroup(message.payload));
						return true;
					}
					
					case RPCMessage::SUBSCRIBE: {
						resender_.endRoutine(message.routine);
						subscribeHost_.completeOperation(message.routine, Empty());
						return true;
					}
					
					case RPCMessage::GET_SUBSCRIBERS: {
						resender_.endRoutine(message.routine);
						getSubscribersHost_.completeOperation(message.routine, readNodeGroup(message.payload));
						return true;
					}
					
					default:
						return false;
				}
			}
			
			RPC::Operation<std::vector<NodeInfo>> RPCClient::getNearestNodes(const NodeId& destId, const NodeId& targetId) {
				const auto routineId = routineIdGenerator_.generateId();
				const auto rpcMessage = RPCMessage::GetNearestNodesRequest(targetId);
				resender_.startRoutine(routineId, destId, rpcMessage.createMessage(routineId));
				return getNearestHost_.startOperation(routineId);
			}
			
			RPC::Operation<Empty> RPCClient::subscribe(const NodeId& destId, const NodeId& targetId, const std::vector<Endpoint>& myEndpoints) {
				const auto routineId = routineIdGenerator_.generateId();
				const auto rpcMessage = RPCMessage::SubscribeRequest(targetId, myEndpoints);
				resender_.startRoutine(routineId, destId, rpcMessage.createMessage(routineId));
				return subscribeHost_.startOperation(routineId);
			}
			
			RPC::Operation<std::vector<NodeInfo>> RPCClient::getSubscribers(const NodeId& destId, const NodeId& targetId) {
				const auto routineId = routineIdGenerator_.generateId();
				const auto rpcMessage = RPCMessage::GetSubscribersRequest(targetId);
				resender_.startRoutine(routineId, destId, rpcMessage.createMessage(routineId));
				return getSubscribersHost_.startOperation(routineId);
			}
			
		}
		
	}
	
}


