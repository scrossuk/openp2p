#include <stdint.h>

#include <p2p/Root/Packet.hpp>

#include <p2p/Transport.hpp>

namespace p2p {

	namespace Root {
	
		PacketHeader ReadPacketHeader(BlockingReader& reader) {
			PacketHeader header;
			
			const uint8_t versionField = Binary::ReadUint8(reader);
			if (versionField != 1) {
				throw std::runtime_error("Invalid version.");
			}
			
			header.version = VERSION_1;
			
			const uint8_t flagsByte = Binary::ReadUint8(reader);
			
			header.state = static_cast<State>((flagsByte >> 6) & 0x03);
			header.sub = (((flagsByte >> 4) & 0x01) == 1);
			header.type = ((flagsByte >> 0) & 0x0F);
			
			header.length = Binary::ReadUint16(reader);
			header.routine = Binary::ReadUint32(reader);
			header.messageCounter = Binary::ReadUint64(reader);
			header.destinationId = NodeId::FromReader(reader);
			
			if (header.sub) {
				header.subnetworkId = NetworkId::FromReader(reader);
			}
			
			return header;
		}
		
		void WritePacketHeader(BlockingWriter& writer, const PacketHeader& header) {
			Binary::WriteUint8(writer, header.version);
			const uint8_t flagsByte = (static_cast<uint8_t>(header.state) << 6) |
				(header.sub ? (1 << 4) : 0) |
				static_cast<uint8_t>(header.type);
			Binary::WriteUint8(writer, flagsByte);
			Binary::WriteUint16(writer, header.length);
			Binary::WriteUint32(writer, header.routine);
			Binary::WriteUint64(writer, header.messageCounter);
			header.destinationId.writeTo(writer);
			
			if (header.sub) {
				header.subnetworkId.writeTo(writer);
			}
		}
		
		Packet ReadPacket(BlockingReader& reader) {
			Packet packet;
			packet.header = ReadPacketHeader(reader);
			
			packet.payload.resize(packet.header.length);
			reader.readAll(packet.payload.data(), packet.payload.size());
			
			return packet;
		}
		
		void WritePacket(BlockingWriter& writer, const Packet& packet) {
			assert(packet.header.length == packet.payload.size());
			
			WritePacketHeader(writer, packet.header);
			writer.writeAll(packet.payload.data(), packet.payload.size());
		}
		
		PacketSignature ReadSignature(BlockingReader& reader) {
			PacketSignature sig;
			sig.signature.resize(SIGNATURE_SIZE_BYTES);
			reader.readAll(sig.signature.data(), sig.signature.size());
			
			Buffer encodedPublicKey;
			encodedPublicKey.resize(PUBLIC_KEY_SIZE_BYTES);
			reader.readAll(encodedPublicKey.data(), encodedPublicKey.size());
			sig.publicKey = PublicKey::FromBuffer(encodedPublicKey);
			return sig;
		}
		
		void WriteSignature(BlockingWriter& writer, const PacketSignature& sig) {
			writer.writeAll(sig.signature.data(), sig.signature.size());
			
			const auto encodedPublicKey = sig.publicKey.toBuffer();
			writer.writeAll(encodedPublicKey.data(), encodedPublicKey.size());
		}
		
	}
	
}
