#include <stdint.h>

#include <OpenP2P/Stream/BinaryStream.hpp>

#include <OpenP2P/RootNetwork/Packet.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		PacketHeader ReadPacketHeader(BlockingReader& reader) {
			PacketHeader header;
			
			const uint8_t versionField = Binary::ReadUint8(reader);
			if (versionField != 1) {
				throw std::runtime_error("Invalid version.");
			}
			
			header.version = VERSION_1;
			
			const uint8_t flagsByte = Binary::ReadUint8(reader);
			
			header.state = static_cast<State>((flagsByte >> 6) & 0x03);
			header.err = (((flagsByte >> 5) & 0x01) == 1);
			header.sub = (((flagsByte >> 4) & 0x01) == 1);
			header.type = ((flagsByte >> 0) & 0x0F);
			
			const uint16_t length = Binary::ReadUint16(reader);
			if (length < MIN_PACKET_SIZE_BYTES) {
				throw std::runtime_error("Packet length is less than minimum.");
			}
			
			header.length = length;
			header.routine = Binary::ReadUint32(reader);
			header.messageCounter = Binary::ReadUint64(reader);
			header.destinationId = NodeId::FromReader(reader);
			return header;
		}
		
		void WritePacketHeader(BlockingWriter& writer, const PacketHeader& header) {
			Binary::WriteUint8(writer, header.version);
			const uint8_t flagsByte = (static_cast<uint8_t>(header.state) << 6) |
				(header.err ? (1 << 5) : 0) |
				(header.sub ? (1 << 4) : 0) |
				static_cast<uint8_t>(header.type);
			Binary::WriteUint8(writer, flagsByte);
			Binary::WriteUint16(writer, header.length);
			Binary::WriteUint32(writer, header.routine);
			Binary::WriteUint64(writer, header.messageCounter);
			header.destinationId.writeTo(writer);
		}
		
		Packet ReadPacket(BlockingReader& reader) {
			Packet packet;
			packet.header = ReadPacketHeader(reader);
			
			const size_t payloadSize = packet.header.length - MIN_PACKET_SIZE_BYTES;
			packet.payload.resize(payloadSize);
			reader.readAll(packet.payload.data(), packet.payload.size());
			return packet;
		}
		
		void WritePacket(BlockingWriter& writer, const Packet& packet) {
			assert(packet.header.length >= MIN_PACKET_SIZE_BYTES);
			assert(packet.header.length == (MIN_PACKET_SIZE_BYTES + packet.payload.size()));
			WritePacketHeader(writer, packet.header);
			writer.writeAll(packet.payload.data(), packet.payload.size());
		}
		
	}
	
}
