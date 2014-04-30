#include <stdexcept>
#include <vector>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NodeId.hpp>

#include <p2p/Root/DHT/RPCMessage.hpp>

namespace p2p {

	namespace Root {
	
		namespace DHT {
			
			RPCMessage RPCMessage::GetNearestNodesRequest(const NodeId& targetId) {
				RPCMessage message(RPCMessage::GET_NEAREST_NODES, STATE_0);
				message.targetId_ = targetId;
				return message;
			}
			
			RPCMessage RPCMessage::GetNearestNodesReply(const std::vector<NodeInfo>& nodeList) {
				RPCMessage message(RPCMessage::GET_NEAREST_NODES, STATE_1);
				message.nodeList_ = nodeList;
				return message;
			}
			
			RPCMessage RPCMessage::SubscribeRequest(const NodeId& targetId, const std::vector<Endpoint>& myEndpoints) {
				RPCMessage message(RPCMessage::SUBSCRIBE, STATE_0);
				message.targetId_ = targetId;
				message.myEndpoints_ = myEndpoints;
				return message;
			}
			
			RPCMessage RPCMessage::SubscribeReply() {
				return RPCMessage(RPCMessage::SUBSCRIBE, STATE_1);
			}
			
			RPCMessage RPCMessage::GetSubscribersRequest(const NodeId& targetId) {
				RPCMessage message(RPCMessage::GET_SUBSCRIBERS, STATE_0);
				message.targetId_ = targetId;
				return message;
			}
			
			RPCMessage RPCMessage::GetSubscribersReply(const std::vector<NodeInfo>& nodeList) {
				RPCMessage message(RPCMessage::GET_SUBSCRIBERS, STATE_1);
				message.nodeList_ = nodeList;
				return message;
			}
			
			RPCMessage::RPCMessage(Kind pKind, State pState)
				: kind_(pKind), state_(pState) { }
				
			RPCMessage::Kind RPCMessage::kind() const {
				return kind_;
			}
			
			State RPCMessage::state() const {
				return state_;
			}
			
			const NodeId& RPCMessage::targetId() const {
				// TODO: error checking!
				return targetId_;
			}
			
			const std::vector<Endpoint>& RPCMessage::endpointList() const {
				// TODO: error checking!
				return myEndpoints_;
			}
			
			const std::vector<NodeInfo>& RPCMessage::nodeList() const {
				// TODO: error checking!
				return nodeList_;
			}
			
			Buffer RPCMessage::toPayload() const {
				Buffer payload;
				BufferBuilder payloadBuilder(payload);
				BinaryOStream writer(payloadBuilder);
				
				switch (kind()) {
					case GET_NEAREST_NODES:
						if (state() == STATE_0) {
							targetId().writeTo(writer);
						} else if (state() == STATE_1) {
							for (const auto& nodeInfo: nodeList()) {
								nodeInfo.writeTo(writer);
							}
						}
						break;
						
					case SUBSCRIBE:
						if (state() == STATE_0) {
							targetId().writeTo(writer);
							Binary::WriteUint16(writer, endpointList().size());
							for (const auto& endpoint: endpointList()) {
								endpoint.writeTo(writer);
							}
						}
						break;
						
					case GET_SUBSCRIBERS:
						if (state() == STATE_0) {
							targetId().writeTo(writer);
						} else if (state() == STATE_1) {
							for (const auto& nodeInfo: nodeList()) {
								nodeInfo.writeTo(writer);
							}
						}
						break;
						
					default:
						break;
				}
				
				return payload;
			}
			
			Message RPCMessage::createMessage(uint32_t routine) const {
				Message message;
				message.subnetwork = boost::make_optional(NetworkId::Generate("p2p.rootdht"));
				message.type = static_cast<uint8_t>(kind());
				message.routine = routine;
				message.routineState = state();
				message.payload = toPayload();
				return message;
			}
			
		}
		
	}
	
}

