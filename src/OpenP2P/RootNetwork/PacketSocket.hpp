#ifndef OPENP2P_ROOTNETWORK_PACKETSOCKET_HPP
#define OPENP2P_ROOTNETWORK_PACKETSOCKET_HPP

#include <assert.h>
#include <stdint.h>

#include <OpenP2P/Buffer.hpp>

#include <OpenP2P/Stream/BinaryStream.hpp>

#include <OpenP2P/Event/Source.hpp>
#include <OpenP2P/Event/Wait.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		class PacketSocket: public Socket<Endpoint, Packet> {
			public:
				inline PacketSocket(Socket<UDP::Endpoint, Buffer>& udpSocket)
					: udpSocket_(udpSocket) { }
				
				inline bool isValid() const {
					return udpSocket_.isValid();
				}
				
				inline Event::Source eventSource() const {
					return udpSocket_.eventSource();
				}
				
				inline bool receive(Endpoint& endpoint, Packet& packet) {
					UDP::Endpoint udpEndpoint;
					Buffer buffer;
					if (!udpSocket_.receive(udpEndpoint, buffer)) {
						return false;
					}
					
					endpoint.kind = Endpoint::UDPIPV6;
					endpoint.udpEndpoint = udpEndpoint;
					
					BufferIterator bufferIterator(buffer);
					BinaryIStream blockingReader(bufferIterator);
					packet = ReadPacket(blockingReader);
					return true;
				}
				
				inline bool send(const Endpoint& endpoint, const Packet& packet) {
					assert(endpoint.kind == Endpoint::UDPIPV6);
					
					Buffer buffer;
					BufferBuilder bufferBuilder(buffer);
					BinaryOStream blockingWriter(bufferBuilder);
					WritePacket(blockingWriter, packet);
					return udpSocket_.send(endpoint.udpEndpoint, buffer);
				}
				
			private:
				Socket<UDP::Endpoint, Buffer>& udpSocket_;
				
		};
		
	}
	
}

#endif
