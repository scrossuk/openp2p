#ifndef P2P_ROOTNETWORK_PACKET_HPP
#define P2P_ROOTNETWORK_PACKET_HPP

#include <stdint.h>

#include <p2p/Root/Root_api.hpp>
#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/Types.hpp>

#include <p2p/Transport.hpp>

namespace p2p {

	namespace Root {
		
		struct PacketHeader {
			Version version; // 8 bits.
			State state; // 2 bits.
			bool sub; // 1 bit.
			MessageType type; // 4 bits.
			PacketLength length; // 16 bits.
			RoutineId routine; // 32 bits.
			MessageCounter messageCounter; // 64 bits.
			NodeId destinationId; // 256 bits.
			NetworkId subnetworkId; // If sub == true, 64 bits.
			
			inline PacketHeader()
				: version(VERSION_INVALID),
				state(STATE_0),
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
