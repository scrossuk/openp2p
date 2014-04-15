#ifndef OPENP2P_ROOTNETWORK_AUTHENTICATEDSOCKET_HPP
#define OPENP2P_ROOTNETWORK_AUTHENTICATEDSOCKET_HPP

#include <OpenP2P/Socket.hpp>

#include <OpenP2P/Event/Source.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/IdentityDatabase.hpp>
#include <OpenP2P/RootNetwork/Message.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		class PrivateIdentity;
		
		class AuthenticatedSocket: public Socket<Endpoint, Message> {
			public:
				AuthenticatedSocket(IdentityDatabase& identityDatabase, PrivateIdentity& privateIdentity, Socket<Endpoint, SignedPacket>& socket);
				
				bool isValid() const;
				
				Event::Source eventSource() const;
				
				bool receive(Endpoint& endpoint, Message& message);
				
				bool send(const Endpoint& endpoint, const Message& message);
				
			private:
				// Non-blocking.
				AuthenticatedSocket(const AuthenticatedSocket&) = delete;
				AuthenticatedSocket& operator=(AuthenticatedSocket) = delete;
				
				IdentityDatabase& identityDatabase_;
				PrivateIdentity& privateIdentity_;
				Socket<Endpoint, SignedPacket>& socket_;
				
		};
		
	}
	
}

#endif
