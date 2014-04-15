#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include <OpenP2P/Buffer.hpp>

#include <OpenP2P/Stream/BinaryStream.hpp>

#include <OpenP2P/Event/Source.hpp>
#include <OpenP2P/Event/Wait.hpp>

#include <OpenP2P/RootNetwork/AuthenticatedSocket.hpp>
#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/IdentityDatabase.hpp>
#include <OpenP2P/RootNetwork/Message.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>
#include <OpenP2P/RootNetwork/PrivateIdentity.hpp>
#include <OpenP2P/RootNetwork/PublicIdentity.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		AuthenticatedSocket::AuthenticatedSocket(IdentityDatabase& identityDatabase, PrivateIdentity& privateIdentity, Socket<Endpoint, SignedPacket>& socket)
			: identityDatabase_(identityDatabase),
			privateIdentity_(privateIdentity),
			socket_(socket) { }
			
		bool AuthenticatedSocket::isValid() const {
			return socket_.isValid();
		}
		
		Event::Source AuthenticatedSocket::eventSource() const {
			return socket_.eventSource();
		}
		
		bool AuthenticatedSocket::receive(Endpoint& endpoint, Message& message) {
			SignedPacket signedPacket;
			
			if (!socket_.receive(endpoint, signedPacket)) {
				return false;
			}
			
			const auto& packet = signedPacket.packet;
			
			auto& publicIdentity = identityDatabase_.getIdentity(signedPacket.signature.publicKey);
			if (!publicIdentity.verify(packet, signedPacket.signature)) {
				return false;
			}
			
			message.sourceId = publicIdentity.id();
			message.destinationId = packet.header.destinationId;
			message.isError = packet.header.err;
			message.subnetwork = packet.header.sub ? boost::make_optional(packet.header.subnetworkId) : boost::none;
			message.type = packet.header.type;
			message.routine = packet.header.routine;
			message.routineState = packet.header.state;
			message.payload = packet.payload;
			return true;
		}
		
		bool AuthenticatedSocket::send(const Endpoint& endpoint, const Message& message) {
			SignedPacket signedPacket;
			auto& packet = signedPacket.packet;
			packet.header.version = VERSION_1;
			packet.header.state = message.routineState;
			packet.header.err = message.isError;
			packet.header.sub = message.subnetwork;
			packet.header.type = message.type;
			packet.header.length = message.payload.size();
			packet.header.routine = message.routine;
			packet.header.messageCounter = privateIdentity_.nextPacketCount();
			packet.header.destinationId = message.destinationId;
			packet.header.subnetworkId = message.subnetwork ? *(message.subnetwork) : NetworkId();
			
			packet.payload = message.payload;
			
			signedPacket.signature = privateIdentity_.sign(signedPacket.packet);
			return socket_.send(endpoint, signedPacket);
		}
		
	}
	
}

