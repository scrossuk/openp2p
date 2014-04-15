#ifndef OPENP2P_ROOTNETWORK_PACKETSOCKET_HPP
#define OPENP2P_ROOTNETWORK_PACKETSOCKET_HPP

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Socket.hpp>

#include <OpenP2P/Event/Source.hpp>

#include <OpenP2P/UDP/Endpoint.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>

namespace OpenP2P {

	namespace RootNetwork {
		
		class PacketSocket: public Socket<Endpoint, SignedPacket> {
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
