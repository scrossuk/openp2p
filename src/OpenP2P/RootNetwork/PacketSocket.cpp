#include <assert.h>
#include <stdint.h>

#include <OpenP2P/Buffer.hpp>

#include <OpenP2P/Stream/BinaryStream.hpp>

#include <OpenP2P/Event/Source.hpp>
#include <OpenP2P/Event/Wait.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>
#include <OpenP2P/RootNetwork/PacketSocket.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
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

