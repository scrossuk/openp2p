#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include <p2p/Transport.hpp>

#include <p2p/Event/Source.hpp>
#include <p2p/Event/Wait.hpp>

#include <p2p/Root/AuthenticatedSocket.hpp>
#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NodePair.hpp>
#include <p2p/Root/Packet.hpp>
#include <p2p/Root/PrivateIdentity.hpp>
#include <p2p/Root/PublicIdentity.hpp>

namespace p2p {

	namespace Root {
	
		AuthenticatedSocket::AuthenticatedSocket(IdentityDelegate& delegate, Socket<Endpoint, SignedPacket>& socket)
			: delegate_(delegate), socket_(socket) { }
			
		bool AuthenticatedSocket::isValid() const {
			return socket_.isValid();
		}
		
		Event::Source AuthenticatedSocket::eventSource() const {
			return socket_.eventSource();
		}
		
		bool AuthenticatedSocket::receive(NodePair& nodePair, Message& message) {
			SignedPacket signedPacket;
			
			if (!socket_.receive(nodePair.endpoint, signedPacket)) {
				return false;
			}
			
			const auto& packet = signedPacket.packet;
			
			if (packet.header.destinationId != delegate_.getPrivateIdentity().id() && packet.header.destinationId != NodeId::Zero()) {
				// Routing not currently supported, so just reject this.
				return false;
			}
			
			auto& publicIdentity = delegate_.getPublicIdentity(signedPacket.signature.publicKey);
			if (!publicIdentity.verify(packet, signedPacket.signature)) {
				return false;
			}
			
			nodePair.id = publicIdentity.id();
			
			message.subnetwork = packet.header.sub ? boost::make_optional(packet.header.subnetworkId) : boost::none;
			message.type = packet.header.type;
			message.routine = packet.header.routine;
			message.routineState = packet.header.state;
			message.payload = packet.payload;
			return true;
		}
		
		bool AuthenticatedSocket::send(const NodePair& nodePair, const Message& message) {
			auto& privateIdentity = delegate_.getPrivateIdentity();
			
			SignedPacket signedPacket;
			auto& packet = signedPacket.packet;
			packet.header.version = VERSION_1;
			packet.header.state = message.routineState;
			packet.header.sub = static_cast<bool>(message.subnetwork);
			packet.header.type = message.type;
			packet.header.length = message.payload.size();
			packet.header.routine = message.routine;
			packet.header.messageCounter = privateIdentity.nextPacketCount();
			packet.header.destinationId = nodePair.id;
			packet.header.subnetworkId = message.subnetwork ? *(message.subnetwork) : NetworkId();
			
			packet.payload = message.payload;
			
			signedPacket.signature = privateIdentity.sign(signedPacket.packet);
			return socket_.send(nodePair.endpoint, signedPacket);
		}
		
	}
	
}

