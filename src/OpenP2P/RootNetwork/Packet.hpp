#ifndef OPENP2P_ROOTNETWORK_PACKET_HPP
#define OPENP2P_ROOTNETWORK_PACKET_HPP

#include <OpenP2P/BinaryStream.hpp>

namespace OpenP2P{

	namespace RootNetwork{

		enum Version{
			VERSION_0 = 0 //Currently only one version
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
			uint8_t version; //4 bits
			bool isRequest; //1 bit
			uint8_t type; //3 bits
			uint8_t packetData; //8 bits
			uint16_t rpcNumber; //16 bits
			Id destinationId; //256 bits
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
			PingRequest pingRequest;
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

		BinaryIStream& operator>>(BinaryIStream&, PacketHeader&);

		BinaryOStream& operator<<(BinaryOStream&, const PacketHeader&);
		
		BinaryIStream& operator>>(BinaryIStream&, Packet&);
		
		BinaryOStream& operator<<(BinaryOStream&, const Packet&);

	}

}

#endif
