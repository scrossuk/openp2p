#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include <OpenP2P/Buffer.hpp>

#include <OpenP2P/Stream/BinaryStream.hpp>

#include <OpenP2P/Event/Source.hpp>
#include <OpenP2P/Event/Wait.hpp>

#include <OpenP2P/RootNetwork/AuthenticatedSocket.hpp>
#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>
#include <OpenP2P/RootNetwork/PrivateIdentity.hpp>
#include <OpenP2P/RootNetwork/PublicIdentity.hpp>
#include <OpenP2P/RootNetwork/SignedPacket.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		AuthenticatedSocket::AuthenticatedSocket(PrivateIdentity& identity, Socket<Endpoint, SignedPacket>& socket)
			: identity_(identity), socket_(socket) { }
			
		bool AuthenticatedSocket::isValid() const {
			return socket_.isValid();
		}
		
		Event::Source AuthenticatedSocket::eventSource() const {
			return socket_.eventSource();
		}
		
		bool AuthenticatedSocket::receive(Endpoint& endpoint, Packet& packet) {
			SignedPacket signedPacket;
			
			if (!socket_.receive(endpoint, signedPacket)) {
				return false;
			}
			
			// TODO: record message counter.
			PublicIdentity publicIdentity(signedPacket.signature.publicKey, 0);
			
			if (!publicIdentity.verify(signedPacket.packet, signedPacket.signature)) {
				printf("Packet signature is invalid.\n");
				return false;
			}
			
			printf("Received signed packet from '%s'.\n",
				   NodeId::Generate(signedPacket.signature.publicKey).hexString().c_str());
				   
			packet = signedPacket.packet;
			
			return true;
		}
		
		bool AuthenticatedSocket::send(const Endpoint& endpoint, const Packet& packet) {
			// TODO: write message counter.
			const auto signature = identity_.sign(packet);
			
			printf("Sending signed packet from '%s'.\n",
				   NodeId::Generate(signature.publicKey).hexString().c_str());
				   
			SignedPacket signedPacket;
			signedPacket.packet = packet;
			signedPacket.signature = signature;
			
			return socket_.send(endpoint, signedPacket);
		}
		
	}
	
}

