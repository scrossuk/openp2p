#ifndef P2P_ROOTNETWORK_AUTHENTICATEDSOCKET_HPP
#define P2P_ROOTNETWORK_AUTHENTICATEDSOCKET_HPP

#include <p2p/Socket.hpp>

#include <p2p/Event/Source.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NodeDatabase.hpp>
#include <p2p/Root/Packet.hpp>

namespace p2p {

	namespace Root {
	
		class PrivateIdentity;
		
		class AuthenticatedSocket: public Socket<Endpoint, Message> {
			public:
				AuthenticatedSocket(NodeDatabase& nodeDatabase, PrivateIdentity& privateIdentity, Socket<Endpoint, SignedPacket>& socket);
				
				bool isValid() const;
				
				Event::Source eventSource() const;
				
				bool receive(Endpoint& endpoint, Message& message);
				
				bool send(const Endpoint& endpoint, const Message& message);
				
			private:
				// Non-blocking.
				AuthenticatedSocket(const AuthenticatedSocket&) = delete;
				AuthenticatedSocket& operator=(AuthenticatedSocket) = delete;
				
				NodeDatabase& nodeDatabase_;
				PrivateIdentity& privateIdentity_;
				Socket<Endpoint, SignedPacket>& socket_;
				
		};
		
	}
	
}

#endif
