#ifndef OPENP2P_ROOTNETWORK_PACKET_HPP
#define OPENP2P_ROOTNETWORK_PACKET_HPP

#include <OpenP2P/BinaryStream.hpp>

#include <OpenP2P/RootNetwork/Node.hpp>
#include <OpenP2P/RootNetwork/NodeId.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
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
		
		enum PacketType {
			TYPE_IDENTIFY = 0,
			TYPE_PING = 1,
			TYPE_QUERY_SUBNETWORKS = 4,
			TYPE_KEY_EXCHANGE = 8
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
			PacketType type; // 4 bits.
			uint16_t length; // 16 bits.
			uint32_t routine; // 32 bits.
			uint64_t messageCounter; // 64 bits.
			NodeId destinationId; // 256 bits.
			
			inline PacketHeader()
				: version(VERSION_INVALID),
				state(STATE_0),
				err(false),
				sub(false),
				type(TYPE_IDENTIFY),
				length(0),
				routine(0),
				messageCounter(0) { }
				
		};
		
		struct IdentifyRequest { };
		
		struct IdentifyReply {
			Endpoint endpoint;
		}
		
		struct PingRequest { };
		
		struct PingReply {
			Endpoint endpoint;
		};
		
		struct QuerySubnetworksRequest { };
		
		struct QuerySubnetworksReply {
			std::vector<NetworkId> networks;
		};
		
		struct KeyExchangeRequest {
			// TODO.
		};
		
		struct KeyExchangeReply {
			// TODO.
		};
		
		// Using struct because union has problems with C++ types.
		struct PacketData {
			IdentifyRequest identifyRequest;
			IdentifyReply identifyReply;
			PingRequest pingRequest;
			PingReply pingReply;
			QuerySubnetworksRequest querySubnetworksRequest;
			QuerySubnetworksReply querySubnetworksReply;
			KeyExchangeRequest keyExchangeRequest;
			KeyExchangeReply keyExchangeReply;
		};
		
		struct Packet {
			PacketHeader header;
			PacketData data;
		};
		
		bool ReadPacketHeader(BinaryIStream& stream, PacketHeader* header, Timeout timeout = Timeout::Infinite());
		
		bool WritePacketHeader(BinaryOStream& stream, const PacketHeader& header, Timeout timeout = Timeout::Infinite());
		
		bool ReadPacket(BinaryIStream& stream, Packet* packet, Timeout timeout = Timeout::Infinite());
		
		bool WritePacket(BinaryOStream& stream, const Packet& packet, Timeout timeout = Timeout::Infinite());
		
	}
	
}

#endif
