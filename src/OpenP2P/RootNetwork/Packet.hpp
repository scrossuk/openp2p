#ifndef OPENP2P_ROOTNETWORK_PACKET_HPP
#define OPENP2P_ROOTNETWORK_PACKET_HPP

#include <OpenP2P/BinaryStream.hpp>

#include <OpenP2P/RootNetwork/Id.hpp>
#include <OpenP2P/RootNetwork/Node.hpp>

namespace OpenP2P{

	namespace RootNetwork{

		enum Version{
			VERSION_0 = 0 // Currently only one version.
		};

		enum PacketType{
			TYPE_PING = 0,
			TYPE_GETSUBNETWORK = 1,
			TYPE_SUBSCRIBE = 2,
			TYPE_NODELOOKUP = 3,
			TYPE_GETSUBSCRIBERS = 4,
			TYPE_PUSHSUBSCRIBERS = 5
		};

		struct PacketHeader{
			Version version; // 8 bits.
			bool isRequest; // 1 bit.
			PacketType type; // 7 bits.
			uint8_t data; // 8 bits.
			uint8_t rpcNumber; // 8 bits.
			Id destinationId; // 256 bits.
		};
		
		struct PingRequest{ };
		
		struct PingReply{ };
		
		struct GetSubnetworkRequest{ Id subnetworkId; };
		
		struct GetSubnetworkReply{ uint8_t contactInfo[256]; };
		
		struct SubscribeRequest{ Id subnetworkId; };
		
		struct SubscribeReply{ };
		
		struct NodeLookupRequest{ Id nodeId; };
		
		struct NodeLookupReply{ boost::array<Node, 16> nodeList; };
		
		struct GetSubscribersRequest{ Id subnetworkId; };
		
		struct GetSubscribersReply{ boost::array<Node, 16> subscriberList; };
		
		struct PushSubscribersRequest{ boost::array<Node, 16> subscriberList; };
		
		struct PushSubscribersReply{ };

		union PacketData{
			PingReply pingReply;
			GetSubnetworkRequest getSubnetworkRequest;
			GetSubnetworkReply getSubnetworkReply;
			SubscribeRequest subscribeRequest;
			SubscribeReply subscribeReply;
			NodeLookupRequest nodeLookupRequest;
			NodeLookupReply nodeLookupReply;
			GetSubscribersRequest getSubscribersRequest;
			GetSubscribersReply getSubscribersReply;
			PushSubscribersRequest pushSubscribersRequest;
			PushSubscribersReply pushSubscribersReply;
		};
		
		struct Packet{
			PacketHeader header;
			PacketData data;
		};
		
		bool ReadPacketHeader(BinaryIStream& stream, PacketHeader * header, Timeout timeout = Timeout::Infinite());
		
		bool WritePacketHeader(BinaryOStream& stream, const PacketHeader& header, Timeout timeout = Timeout::Infinite());
		
		bool ReadPacket(BinaryIStream& stream, Packet * packet, Timeout timeout = Timeout::Infinite());
		
		bool WritePacket(BinaryOStream& stream, const Packet& packet, Timeout timeout = Timeout::Infinite());

	}

}

#endif
