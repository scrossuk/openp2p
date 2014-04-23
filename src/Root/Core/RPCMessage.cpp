#include <stdexcept>
#include <vector>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NodeId.hpp>

#include <p2p/Root/Core/RPCMessage.hpp>

namespace p2p {

	namespace Root {
	
		namespace Core {
		
			RPCMessage RPCMessage::IdentifyRequest() {
				return RPCMessage(RPCMessage::IDENTIFY, STATE_0);
			}
			
			RPCMessage RPCMessage::IdentifyReply(const Endpoint& endpoint) {
				RPCMessage message(RPCMessage::IDENTIFY, STATE_1);
				message.endpoint_ = endpoint;
				return message;
			}
			
			RPCMessage RPCMessage::PingRequest() {
				return RPCMessage(RPCMessage::PING, STATE_0);
			}
			
			RPCMessage RPCMessage::PingReply(const Endpoint& endpoint) {
				RPCMessage message(RPCMessage::PING, STATE_1);
				message.endpoint_ = endpoint;
				return message;
			}
			
			RPCMessage RPCMessage::QueryNetworksRequest() {
				return RPCMessage(RPCMessage::QUERY_NETWORKS, STATE_0);
			}
			
			RPCMessage RPCMessage::QueryNetworksReply(const std::vector<NetworkId>& networks) {
				RPCMessage message(RPCMessage::QUERY_NETWORKS, STATE_1);
				message.networks_ = networks;
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
			
			const Endpoint& RPCMessage::endpoint() const {
				if ((kind() != RPCMessage::IDENTIFY && kind() != RPCMessage::PING) || state() != STATE_1) {
					throw std::runtime_error("Invalid access to RPCMessage::endpoint().");
				}
				
				return endpoint_;
			}
			
			const std::vector<NetworkId>& RPCMessage::networks() const {
				if (kind() != RPCMessage::QUERY_NETWORKS || state() != STATE_1) {
					throw std::runtime_error("Invalid access to RPCMessage::networks().");
				}
				
				return networks_;
			}
			
			Buffer RPCMessage::toPayload() const {
				Buffer payload;
				BufferBuilder payloadBuilder(payload);
				BinaryOStream writer(payloadBuilder);
				
				switch (kind()) {
					case IDENTIFY:
						if (state() == STATE_1) {
							endpoint().writeTo(writer);
						}
						
						break;
						
					case PING:
						if (state() == STATE_1) {
							endpoint().writeTo(writer);
						}
						
						break;
						
					case QUERY_NETWORKS:
						if (state() == STATE_1) {
							for (size_t i = 0; i < networks().size(); i++) {
								networks().at(i).writeTo(writer);
							}
						}
						
						break;
						
					case KEY_EXCHANGE:
						if (state() == STATE_1) {
							// TODO
						}
						
						break;
						
					default:
						break;
				}
				
				return payload;
			}
			
			Message RPCMessage::createMessage(uint32_t routine) const {
				Message message;
				message.subnetwork = boost::none;
				message.type = static_cast<uint8_t>(kind());
				message.routine = routine;
				message.routineState = state();
				message.payload = toPayload();
				return message;
			}
			
		}
		
	}
	
}

