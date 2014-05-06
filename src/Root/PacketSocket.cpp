#include <assert.h>
#include <stdint.h>

#include <p2p/Event/Source.hpp>
#include <p2p/Event/Wait.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Packet.hpp>
#include <p2p/Root/PacketSocket.hpp>

#include <p2p/Transport.hpp>

namespace p2p {

	namespace Root {
	
		PacketSocket::PacketSocket(Socket<Endpoint, Buffer>& socket)
			: socket_(socket) { }
			
		bool PacketSocket::isValid() const {
			return socket_.isValid();
		}
		
		Event::Source PacketSocket::eventSource() const {
			return socket_.eventSource();
		}
		
		bool PacketSocket::receive(Endpoint& endpoint, SignedPacket& signedPacket) {
			Buffer buffer;
			
			if (!socket_.receive(endpoint, buffer)) {
				return false;
			}
			
			BufferIterator bufferIterator(buffer);
			BinaryIStream blockingReader(bufferIterator);
			signedPacket.packet = ReadPacket(blockingReader);
			signedPacket.signature = ReadSignature(blockingReader);
			return true;
		}
		
		bool PacketSocket::send(const Endpoint& endpoint, const SignedPacket& signedPacket) {
			Buffer buffer;
			BufferBuilder bufferBuilder(buffer);
			BinaryOStream blockingWriter(bufferBuilder);
			WritePacket(blockingWriter, signedPacket.packet);
			WriteSignature(blockingWriter, signedPacket.signature);
			return socket_.send(endpoint, buffer);
		}
		
	}
	
}

