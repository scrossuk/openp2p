#ifndef OPENP2P_ROOTNETWORK_AUTHENTICATEDSOCKET_HPP
#define OPENP2P_ROOTNETWORK_AUTHENTICATEDSOCKET_HPP

#include <assert.h>
#include <stdint.h>

#include <OpenP2P/Buffer.hpp>

#include <OpenP2P/Stream/BinaryStream.hpp>

#include <OpenP2P/Event/Source.hpp>
#include <OpenP2P/Event/Wait.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>
#include <OpenP2P/RootNetwork/PrivateIdentity.hpp>
#include <OpenP2P/RootNetwork/PublicIdentity.hpp>
#include <OpenP2P/RootNetwork/SignedPacket.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		class AuthenticatedSocket: public Socket<Endpoint, Packet> {
			public:
				inline AuthenticatedSocket(PrivateIdentity& identity, Socket<Endpoint, SignedPacket>& socket)
					: identity_(identity), socket_(socket) { }
				
				inline bool isValid() const {
					return socket_.isValid();
				}
				
				inline Event::Source eventSource() const {
					return socket_.eventSource();
				}
				
				inline bool receive(Endpoint& endpoint, Packet& packet) {
					SignedPacket signedPacket;
					if (!socket_.receive(endpoint, signedPacket)) {
						return false;
					}
					
					// TODO: record message counter.
					PublicIdentity publicIdentity(signedPacket.signature.publicKey, 0);
					
					if (!publicIdentity.verify(signedPacket.packet, signedPacket.signature)) {
						return false;
					}
					
					packet = signedPacket.packet;
					
					return true;
				}
				
				inline bool send(const Endpoint& endpoint, const Packet& packet) {
					// TODO: write message counter.
					const auto signature = identity_.sign(packet);
					
					SignedPacket signedPacket;
					signedPacket.packet = packet;
					signedPacket.signature = signature;
					return socket_.send(endpoint, signedPacket);
				}
				
			private:
				PrivateIdentity& identity_;
				Socket<Endpoint, SignedPacket>& socket_;
				
		};
		
	}
	
}

#endif
