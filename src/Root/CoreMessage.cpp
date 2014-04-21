#include <stdexcept>
#include <vector>

#include <p2p/Root/CoreMessage.hpp>
#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/Packet.hpp>

namespace p2p {

	namespace Root {
	
		CoreMessage CoreMessage::IdentifyRequest() {
			return CoreMessage(CoreMessage::IDENTIFY, STATE_0);
		}
		
		CoreMessage CoreMessage::IdentifyReply(const Endpoint& endpoint) {
			CoreMessage message(CoreMessage::IDENTIFY, STATE_1);
			message.endpoint_ = endpoint;
			return message;
		}
		
		CoreMessage CoreMessage::PingRequest() {
			return CoreMessage(CoreMessage::PING, STATE_0);
		}
		
		CoreMessage CoreMessage::PingReply(const Endpoint& endpoint) {
			CoreMessage message(CoreMessage::PING, STATE_1);
			message.endpoint_ = endpoint;
			return message;
		}
		
		CoreMessage CoreMessage::QueryNetworksRequest() {
			return CoreMessage(CoreMessage::QUERY_NETWORKS, STATE_0);
		}
		
		CoreMessage CoreMessage::QueryNetworksReply(const std::vector<NetworkId>& networks) {
			CoreMessage message(CoreMessage::QUERY_NETWORKS, STATE_1);
			message.networks_ = networks;
			return message;
		}
		
		CoreMessage::CoreMessage(Kind pKind, State pState)
			: kind_(pKind), state_(pState) { }
		
		CoreMessage::Kind CoreMessage::kind() const {
			return kind_;
		}
		
		State CoreMessage::state() const {
			return state_;
		}
		
		const Endpoint& CoreMessage::endpoint() const {
			if ((kind() != CoreMessage::IDENTIFY && kind() != CoreMessage::PING) || state() != STATE_1) {
				throw std::runtime_error("Invalid access to CoreMessage::endpoint().");
			}
			return endpoint_;
		}
		
		const std::vector<NetworkId>& CoreMessage::networks() const {
			if (kind() != CoreMessage::QUERY_NETWORKS || state() != STATE_1) {
				throw std::runtime_error("Invalid access to CoreMessage::networks().");
			}
			return networks_;
		}
		
		Buffer CoreMessage::toPayload() const {
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
		
		Message CoreMessage::createMessage(uint32_t routine, const NodeId& sourceId, const NodeId& destinationId) const {
			Message message;
			message.sourceId = sourceId;
			message.destinationId = destinationId;
			message.isError = false;
			message.subnetwork = boost::none;
			message.type = static_cast<uint8_t>(kind());
			message.routine = routine;
			message.routineState = state();
			message.payload = toPayload();
			return message;
		}
		
	}
	
}

