#include <assert.h>
#include <stdint.h>

#include <OpenP2P/Buffer.hpp>

#include <OpenP2P/Stream/BinaryStream.hpp>

#include <OpenP2P/Event/Source.hpp>
#include <OpenP2P/Event/Wait.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>
#include <OpenP2P/RootNetwork/PacketSocket.hpp>
#include <OpenP2P/RootNetwork/SignedPacket.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		PacketSocket::PacketSocket(Socket<UDP::Endpoint, Buffer>& udpSocket)
			: udpSocket_(udpSocket) { }
			
		bool PacketSocket::isValid() const {
			return udpSocket_.isValid();
		}
		
		Event::Source PacketSocket::eventSource() const {
			return udpSocket_.eventSource();
		}
		
		bool PacketSocket::receive(Endpoint& endpoint, SignedPacket& signedPacket) {
			UDP::Endpoint udpEndpoint;
			Buffer buffer;
			
			if (!udpSocket_.receive(udpEndpoint, buffer)) {
				return false;
			}
			
			endpoint.kind = Endpoint::UDPIPV6;
			endpoint.udpEndpoint = udpEndpoint;
			
			BufferIterator bufferIterator(buffer);
			BinaryIStream blockingReader(bufferIterator);
			signedPacket.packet = ReadPacket(blockingReader);
			signedPacket.signature = ReadSignature(blockingReader);
			return true;
		}
		
		bool PacketSocket::send(const Endpoint& endpoint, const SignedPacket& signedPacket) {
			assert(endpoint.kind == Endpoint::UDPIPV6);
			
			Buffer buffer;
			BufferBuilder bufferBuilder(buffer);
			BinaryOStream blockingWriter(bufferBuilder);
			WritePacket(blockingWriter, signedPacket.packet);
			WriteSignature(blockingWriter, signedPacket.signature);
			return udpSocket_.send(endpoint.udpEndpoint, buffer);
		}
		
	}
	
}

