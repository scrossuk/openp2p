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
			TYPE_IDENTIFY = 0,
			TYPE_PING = 1,
			TYPE_GETNEARESTNODES = 2,
			TYPE_SUBSCRIBE = 3,
			TYPE_GETSUBSCRIBERS = 4,
			TYPE_SENDMESSAGE = 5,
			TYPE_OPENSTREAM = 6,
			TYPE_CLOSESTREAM = 7,
			TYPE_SENDSTREAMDATA = 8
		};
		
		enum Status{
			STATUS_SUCCESS = 0,
			STATUS_SUBNETWORKUNSUPPORTED = 1,
			STATUS_TOOMANYCONNECTIONS = 2
		};

		struct PacketHeader{
			Version version; // 4 bits.
			uint8_t state; // 2 bits.
			bool ERR; // 1 bit.
			bool COM; // 1 bit.
			PacketType type; // 8 bits.
			uint16_t data; // 16 bits.
			uint64_t requestCounter; // 64 bits.
			Id destinationId; // 256 bits.
		};
		
		struct IdentifyRequest{ };
		
		struct IdentifyReply{ Endpoint endpoint; }
		
		struct PingRequest{ };
		
		struct PingReply{ Endpoint endpoint; };
		
		struct GetNearestNodesRequest{ Id nodeId; }
		
		struct GetNearestNodesReply{ std::vector<Node> nodeList; }
		
		struct SubscribeRequest{ Id subnetworkId; };
		
		struct SubscribeReply{ };
		
		struct GetSubscribersRequest{ Id subnetworkId; };
		
		struct GetSubscribersReply{ std::vector<Node> subscriberList; };
		
		struct SendMessageRequest{ Id subnetworkId; Buffer data; }
		
		struct SendMessageReply{ Status status; }
		
		struct OpenStreamRequest{ Id subnetworkId; }
		
		struct OpenStreamReply{ Status status; Id streamId; }
		
		struct CloseStreamRequest{ Id streamId; }
		
		struct CloseStreamReply{ Status status; }
		
		struct SendStreamDataRequest{ Id streamId; Buffer data; }
		
		struct SendStreamDataReply{ Status status; }

		// Using struct because union has problems with C++ types.
		struct PacketData{
			IdentifyRequest identifyRequest;
			IdentifyReply identifyReply;
			PingRequest pingRequest;
			PingReply pingReply;
			GetNearestNodesRequest getNearestNodesRequest;
			GetNearestNodesReply getNearestNodesReply;
			SubscribeRequest subscribeRequest;
			SubscribeReply subscribeReply;
			GetSubscribersRequest getSubscribersRequest;
			GetSubscribersReply getSubscribersReply;
			SendMessageRequest sendMessageRequest;
			SendMessageReply sendMessageReply;
			OpenStreamRequest openStreamRequest;
			OpenStreamReply openStreamReply;
			CloseStreamRequest closeStreamRequest;
			CloseStreamReply closeStreamReply;
			SendStreamDataRequest sendStreamDataRequest;
			SendStreamDataReply sendStreamDataReply;
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
