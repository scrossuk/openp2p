#include <stdexcept>
#include <vector>

#include <OpenP2P/RootNetwork/CoreMessage.hpp>
#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/NodeId.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
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
		
		Packet CoreMessage::createPacket(uint32_t routine, const NodeId& destinationId) const {
			Packet packet;
			packet.payload = toPayload();
			packet.header.version = VERSION_1;
			packet.header.state = state();
			packet.header.err = false;
			packet.header.sub = false;
			packet.header.type = static_cast<uint8_t>(kind());
			packet.header.length = MIN_PACKET_SIZE_BYTES + packet.payload.size();
			packet.header.routine = routine;
			
			// This will be set when the packet is signed.
			packet.header.messageCounter = 0;
			
			packet.header.destinationId = destinationId;
			return packet;
		}
		
	}
	
}

