#ifndef P2P_ROOTNETWORK_PACKETSOCKET_HPP
#define P2P_ROOTNETWORK_PACKETSOCKET_HPP

#include <p2p/Root/Root_api.hpp>
#include <p2p/Transport.hpp>
#include <p2p/Transport/Socket.hpp>

#include <p2p/Event/Source.hpp>

#include <p2p/UDP/Endpoint.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Packet.hpp>

namespace p2p {

	namespace Root {
		
		class ROOT_OPENP2P_API PacketSocket: public Socket<Endpoint, SignedPacket> {
			public:
				PacketSocket(Socket<Endpoint, Buffer>& socket);
				
				bool isValid() const;
				
				Event::Source eventSource() const;
				
				bool receive(Endpoint& endpoint, SignedPacket& packet);
				
				bool send(const Endpoint& endpoint, const SignedPacket& packet);
				
			private:
				PacketSocket(const PacketSocket&) = delete;
				PacketSocket& operator=(PacketSocket) = delete;
				
				Socket<Endpoint, Buffer>& socket_;
				
		};
		
	}
	
}

#endif
