#include <stdint.h>

#include <OpenP2P/BinaryStream.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>

namespace OpenP2P{

	namespace RootNetwork{
	
		BinaryIStream& operator>>(BinaryIStream& stream, PacketHeader& header){
			uint8_t firstByte = 0;
			stream >> firstByte >> header.packetData >> header.rpcNumber >> header.destinationId;
			header.version = Version((firstByte >> 4) & 0x0F);
			header.isRequest = (firstByte >> 3) & 0x01;
			header.type = PacketType(firstByte & 0x07); //get last 3 bits
			return stream;
		}

		BinaryOStream& operator<<(BinaryOStream& stream, const PacketHeader& header){
			uint8_t firstByte = (uint8_t(header.version) << 4) | (uint8_t(header.isRequest) << 3) | uint8_t(header.type);
			return stream << firstByte << header.packetData << header.rpcNumber << header.destinationId;
		}
		
		namespace{
		
			void readNodes(BinaryIStream& stream, boost::array<Node, 16>& nodeArray, uint8_t packetData){
				uint8_t ipv4Num = (packetData >> 4) & 0x0F;
				uint8_t totalNum = packetData & 0x0F;
						
				uint8_t pos = 0;
						
				// Read IPv4 addresses.
				for(; pos < ipv4Num; pos++){
					Id nodeId;
					IP::AddressV4 nodeAddress;
					uint16_t nodePort;
					stream >> nodeId >> nodeAddress >> nodePort;
					nodeArray[pos] = Node(nodeId, Endpoint(nodeAddress, nodePort));
				}
						
				// Read IPv6 addresses.
				for(; pos < totalNum; pos++){
					Id nodeId;
					IP::AddressV6 nodeAddress;
					uint16_t nodePort;
					stream >> nodeId >> nodeAddress >> nodePort;
					nodeArray[pos] = Node(nodeId, Endpoint(nodeAddress, nodePort));
				}
			}
		
		}
		
		BinaryIStream& operator>>(BinaryIStream& stream, Packet& packet){
			stream >> packet.header;
			switch(packet.header.type){
				case TYPE_PING:
					return stream;
				case TYPE_GETSUBNETWORK:
					if(packet.header.isRequest){
						return stream >> packet.data.getSubnetworkRequest.subnetworkId;
					}else{
						std::size_t size = packet.header.packetData;
						stream.read(packet.data.getSubnetworkReply.contactInfo, size);
						return stream;
					}
				case TYPE_SUBSCRIBE:
					if(packet.header.isRequest){
						return stream >> packet.data.subscribeRequest.subnetworkId;
					}else{
						return stream;
					}
				case TYPE_NODELOOKUP:
					if(packet.header.isRequest){
						return stream >> packet.data.nodeLookupRequest.nodeId;
					}else{
						readNodes(stream, packet.data.nodeLookupReply.nodeList, packet.header.packetData);
						return stream;
					}
				case TYPE_GETSUBSCRIBERS:
					if(packet.header.isRequest){
						return stream >> packet.data.getSubscribersRequest.subnetworkId;
					}else{
						readNodes(stream, packet.data.getSubscribersReply.subscriberList, packet.header.packetData);
						return stream;
					}
				case TYPE_PUSHSUBSCRIBERS:
					if(packet.header.isRequest){
						readNodes(stream, packet.data.pushSubscribersRequest.subscriberList, packet.header.packetData);
						return stream;
					}else{
						return stream;
					}
				default:
					return stream;
			}
		}
		
		namespace{
		
			void writeNodes(BinaryOStream& stream, const boost::array<Node, 16>& nodeArray, uint8_t packetData){
				uint8_t ipv4Num = (packetData >> 4) & 0x0F;
				uint8_t totalNum = packetData & 0x0F;
				
				uint8_t pos = 0;
				
				// Write IPv4 addresses.
				for(; pos < ipv4Num; pos++){
					const Node& node = nodeArray[pos];
					stream << node.id << node.endpoint.addressV4() << node.endpoint.port();
				}
						
				// Write IPv6 addresses.
				for(; pos < totalNum; pos++){
					const Node& node = nodeArray[pos];
					stream << node.id << node.endpoint.addressV6() << node.endpoint.port();
				}
			}
		
		}
		
		BinaryOStream& operator<<(BinaryOStream& stream, const Packet& packet){
			stream << packet.header;
			switch(packet.header.type){
				case TYPE_PING:
					return stream;
				case TYPE_GETSUBNETWORK:
					if(packet.header.isRequest){
						return stream << packet.data.getSubnetworkRequest.subnetworkId;
					}else{
						std::size_t size = packet.header.packetData;
						stream.write(packet.data.getSubnetworkReply.contactInfo, size);
						return stream;
					}
				case TYPE_SUBSCRIBE:
					if(packet.header.isRequest){
						return stream << packet.data.subscribeRequest.subnetworkId;
					}else{
						return stream;
					}
				case TYPE_NODELOOKUP:
					if(packet.header.isRequest){
						return stream << packet.data.nodeLookupRequest.nodeId;
					}else{
						writeNodes(stream, packet.data.nodeLookupReply.nodeList, packet.header.packetData);
						return stream;
					}
				case TYPE_GETSUBSCRIBERS:
					if(packet.header.isRequest){
						return stream << packet.data.getSubscribersRequest.subnetworkId;
					}else{
						writeNodes(stream, packet.data.getSubscribersReply.subscriberList, packet.header.packetData);
						return stream;
					}
				case TYPE_PUSHSUBSCRIBERS:
					if(packet.header.isRequest){
						writeNodes(stream, packet.data.pushSubscribersRequest.subscriberList, packet.header.packetData);
						return stream;
					}else{
						return stream;
					}
					return stream;
				default:
					return stream;
			}
		}
	
	}

}
