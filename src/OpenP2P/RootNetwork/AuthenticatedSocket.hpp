#ifndef OPENP2P_ROOTNETWORK_AUTHENTICATEDSOCKET_HPP
#define OPENP2P_ROOTNETWORK_AUTHENTICATEDSOCKET_HPP

#include <OpenP2P/Socket.hpp>

#include <OpenP2P/Event/Source.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>
#include <OpenP2P/RootNetwork/SignedPacket.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		class PrivateIdentity;
		
		class AuthenticatedSocket: public Socket<Endpoint, Packet> {
			public:
				AuthenticatedSocket(PrivateIdentity& identity, Socket<Endpoint, SignedPacket>& socket);
				
				bool isValid() const;
				
				Event::Source eventSource() const;
				
				bool receive(Endpoint& endpoint, Packet& packet);
				
				bool send(const Endpoint& endpoint, const Packet& packet);
				
			private:
				// Non-blocking.
				AuthenticatedSocket(const AuthenticatedSocket&) = delete;
				AuthenticatedSocket& operator=(AuthenticatedSocket) = delete;
				
				PrivateIdentity& identity_;
				Socket<Endpoint, SignedPacket>& socket_;
				
		};
		
	}
	
}

#endif
