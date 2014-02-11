#ifndef OPENP2P_ROOTNETWORK_PACKETSOCKET_HPP
#define OPENP2P_ROOTNETWORK_PACKETSOCKET_HPP

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Socket.hpp>

#include <OpenP2P/Event/Source.hpp>

#include <OpenP2P/UDP/Endpoint.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/SignedPacket.hpp>

namespace OpenP2P {

	namespace RootNetwork {
		
		class PacketSocket: public Socket<Endpoint, SignedPacket> {
			public:
				PacketSocket(Socket<UDP::Endpoint, Buffer>& udpSocket);
				
				bool isValid() const;
				
				Event::Source eventSource() const;
				
				bool receive(Endpoint& endpoint, SignedPacket& signedPacket);
				
				bool send(const Endpoint& endpoint, const SignedPacket& signedPacket);
				
			private:
				PacketSocket(const PacketSocket&) = delete;
				PacketSocket& operator=(PacketSocket) = delete;
				
				Socket<UDP::Endpoint, Buffer>& udpSocket_;
				
		};
		
	}
	
}

#endif
