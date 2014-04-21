#ifndef P2P_ROOTNETWORK_PACKET_HPP
#define P2P_ROOTNETWORK_PACKET_HPP

#include <stdint.h>

#include <p2p/Buffer.hpp>

#include <p2p/Stream/BinaryStream.hpp>

#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/NodeId.hpp>

namespace p2p {

	namespace Root {
	
		enum Version {
			VERSION_INVALID = 0,
			VERSION_1 = 1 // Currently only one version.
		};
		
		enum State {
			STATE_0 = 0,
			STATE_1 = 1,
			STATE_2 = 2,
			STATE_3 = 3
		};
		
		enum Error {
			ERROR_INVALID_MESSAGE_FORMAT = 0,
			ERROR_INVALID_SIGNATURE = 1,
			ERROR_INVALID_COUNTER = 2,
			ERROR_VERSION_NOT_SUPPORTED = 3,
			ERROR_UNKNOWN_SUBNETWORK = 8
		};
		
		struct PacketHeader {
			Version version; // 8 bits.
			State state; // 2 bits.
			bool err; // 1 bit.
			bool sub; // 1 bit.
			uint8_t type; // 4 bits.
			uint16_t length; // 16 bits.
			uint32_t routine; // 32 bits.
			uint64_t messageCounter; // 64 bits.
			NodeId destinationId; // 256 bits.
			NetworkId subnetworkId; // If sub == true, 64 bits.
			
			inline PacketHeader()
				: version(VERSION_INVALID),
				state(STATE_0),
				err(false),
				sub(false),
				type(0),
				length(0),
				routine(0),
				messageCounter(0) { }
				
		};
		
		struct PacketSignature {
			Buffer signature;
			PublicKey publicKey;
		};
		
		struct Packet {
			PacketHeader header;
			Buffer payload;
		};
		
		struct SignedPacket {
			Packet packet;
			PacketSignature signature;
		};
		
		PacketHeader ReadPacketHeader(BlockingReader& reader);
		
		void WritePacketHeader(BlockingWriter& writer, const PacketHeader& header);
		
		Packet ReadPacket(BlockingReader& reader);
		
		void WritePacket(BlockingWriter& writer, const Packet& packet);
		
		PacketSignature ReadSignature(BlockingReader& reader);
		
		void WriteSignature(BlockingWriter& writer, const PacketSignature& sig);
		
	}
	
}

#endif
