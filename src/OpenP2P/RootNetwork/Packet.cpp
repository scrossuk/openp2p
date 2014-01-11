#include <stdint.h>

#include <OpenP2P/BinaryStream.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		bool ReadPacketHeader(BinaryIStream& stream, PacketHeader* header, Timeout timeout) {
			TimeoutSequence sequence(timeout);
			
			uint8_t firstByte;
			
			if (!Binary::ReadUint8(stream, &firstByte, sequence.getTimeout())) {
				return false;
			}
			
			header->version = Version((firstByte >> 4) & 0x0F);
			header->state = (firstByte >> 2) & 0x03;
			header->ERR = bool((firstByte >> 1) & 0x01);
			header->COM = bool(firstByte & 0x01);
			
			uint8_t packetTypeByte;
			
			if (!Binary::ReadUint8(stream, &packetTypeByte, sequence.getTimeout())) {
				return false;
			}
			
			header->type = PacketType(packetTypeByte);
			
			if (!Binary::ReadUint16(stream, &(header->data), sequence.getTimeout())) {
				return false;
			}
			
			if (!Binary::ReadUint64(stream, &(header->requestCounter), sequence.getTimeout())) {
				return false;
			}
			
			if (!ReadId(stream, &(header->destinationId), sequence.getTimeout())) {
				return false;
			}
			
			return true;
		}
		
		bool WritePacketHeader(BinaryOStream& stream, const PacketHeader& header, Timeout timeout) {
			TimeoutSequence sequence(timeout);
			
			const uint8_t firstByte = (uint8_t(header.version) << 4)
									  | (header.state << 2)
									  | (uint8_t(header.ERR) << 1)
									  | uint8_t(header.COM);
									  
			if (!Binary::WriteUint8(stream, firstByte, sequence.getTimeout())) {
				return false;
			}
			
			if (!Binary::WriteUint8(stream, uint8_t(header.type), sequence.getTimeout())) {
				return false;
			}
			
			if (!Binary::WriteUint16(stream, header.data, sequence.getTimeout())) {
				return false;
			}
			
			if (!Binary::WriteUint64(stream, header.requestCounter, sequence.getTimeout())) {
				return false;
			}
			
			if (!WriteId(stream, header.destinationId, sequence.getTimeout())) {
				return false;
			}
			
			return true;
		}
		
		bool ReadPacket(BinaryIStream& stream, Packet* packet, Timeout timeout) {
			TimeoutSequence sequence(timeout);
			
			if (!ReadPacketHeader(stream, &(packet->header), sequence.getTimeout())) {
				return false;
			}
			
			switch (packet.header.type) {
				case TYPE_IDENTIFY: {
					if (packet->header.state == 1) {
						return ReadEndpoint(stream, &(packet->data.identifyReply.endpoint), sequence.getTimeout());
					} else {
						return packet->header.state == 0;
					}
				}
				
				case TYPE_PING: {
					if (packet->header.state == 1) {
						return ReadEndpoint(stream, &(packet->data.pingReply.endpoint), sequence.getTimeout());
					} else {
						return packet->header.state == 0;
					}
				}
				
				case TYPE_GETNEARESTNODES: {
			
					break;
				}
				
				case TYPE_SUBSCRIBE: {
			
					break;
				}
				
				case TYPE_GETSUBSCRIBERS: {
			
					break;
				}
				
				case TYPE_SENDMESSAGE: {
			
					break;
				}
				
				case TYPE_OPENSTREAM: {
			
					break;
				}
				
				case TYPE_CLOSESTREAM: {
			
					break;
				}
				
				case TYPE_SENDSTREAMDATA: {
			
					break;
				}
				
				/*case TYPE_PING:
					return true;
				case TYPE_GETSUBNETWORK:
					if(packet.header.isRequest){
						return ReadSubNetworkId(stream, &(packet.data.getSubnetworkRequest.subNetworkId), sequence.getTimeout());
					}else{
						const uint8_t contactInfoSize = packet.header.packetData;
						return stream.read(packet.data.getSubnetworkReply.contactInfo, contactInfoSize, sequence.getTimeout());
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
					}*/
				default:
					return false;
			}
		}
		
		bool WritePacket(BinaryOStream& stream, const Packet& packet, Timeout timeout) {
			TimeoutSequence sequence(timeout);
			
			if (!WritePacketHeader(stream, packet.header, sequence.getTimeout())) {
				return false;
			}
			
			switch (packet.header.type) {
				/*case TYPE_PING:
					return stream;
				case TYPE_GETSUBNETWORK:
					if(packet.header.isRequest){
						return stream << packet.data.getSubnetworkRequest.subnetworkId;
					}else{
						size_t size = packet.header.packetData;
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
					return stream;*/
				default:
					return false;
			}
		}
		
		namespace {
		
			void readNodes(BinaryIStream& stream, boost::array<Node, 16>& nodeArray, uint8_t packetData) {
				uint8_t ipv4Num = (packetData >> 4) & 0x0F;
				uint8_t totalNum = packetData & 0x0F;
				
				uint8_t pos = 0;
				
				// Read IPv4 addresses.
				for (; pos < ipv4Num; pos++) {
					Id nodeId;
					IP::AddressV4 nodeAddress;
					uint16_t nodePort;
					stream >> nodeId >> nodeAddress >> nodePort;
					nodeArray[pos] = Node(nodeId, Endpoint(nodeAddress, nodePort));
				}
				
				// Read IPv6 addresses.
				for (; pos < totalNum; pos++) {
					Id nodeId;
					IP::AddressV6 nodeAddress;
					uint16_t nodePort;
					stream >> nodeId >> nodeAddress >> nodePort;
					nodeArray[pos] = Node(nodeId, Endpoint(nodeAddress, nodePort));
				}
			}
			
		}
		
		BinaryIStream& operator>>(BinaryIStream& stream, Packet& packet) {
			stream >> packet.header;
			
			switch (packet.header.type) {
				case TYPE_PING:
					return stream;
					
				case TYPE_GETSUBNETWORK:
					if (packet.header.isRequest) {
						return stream >> packet.data.getSubnetworkRequest.subnetworkId;
					} else {
						size_t size = packet.header.packetData;
						stream.read(packet.data.getSubnetworkReply.contactInfo, size);
						return stream;
					}
					
				case TYPE_SUBSCRIBE:
					if (packet.header.isRequest) {
						return stream >> packet.data.subscribeRequest.subnetworkId;
					} else {
						return stream;
					}
					
				case TYPE_NODELOOKUP:
					if (packet.header.isRequest) {
						return stream >> packet.data.nodeLookupRequest.nodeId;
					} else {
						readNodes(stream, packet.data.nodeLookupReply.nodeList, packet.header.packetData);
						return stream;
					}
					
				case TYPE_GETSUBSCRIBERS:
					if (packet.header.isRequest) {
						return stream >> packet.data.getSubscribersRequest.subnetworkId;
					} else {
						readNodes(stream, packet.data.getSubscribersReply.subscriberList, packet.header.packetData);
						return stream;
					}
					
				case TYPE_PUSHSUBSCRIBERS:
					if (packet.header.isRequest) {
						readNodes(stream, packet.data.pushSubscribersRequest.subscriberList, packet.header.packetData);
						return stream;
					} else {
						return stream;
					}
					
				default:
					return stream;
			}
		}
		
		namespace {
		
			void writeNodes(BinaryOStream& stream, const boost::array<Node, 16>& nodeArray, uint8_t packetData) {
				uint8_t ipv4Num = (packetData >> 4) & 0x0F;
				uint8_t totalNum = packetData & 0x0F;
				
				uint8_t pos = 0;
				
				// Write IPv4 addresses.
				for (; pos < ipv4Num; pos++) {
					const Node& node = nodeArray[pos];
					stream << node.id << node.endpoint.addressV4() << node.endpoint.port();
				}
				
				// Write IPv6 addresses.
				for (; pos < totalNum; pos++) {
					const Node& node = nodeArray[pos];
					stream << node.id << node.endpoint.addressV6() << node.endpoint.port();
				}
			}
			
		}
		
		BinaryOStream& operator<<(BinaryOStream& stream, const Packet& packet) {
			stream << packet.header;
			
			switch (packet.header.type) {
				case TYPE_PING:
					return stream;
					
				case TYPE_GETSUBNETWORK:
					if (packet.header.isRequest) {
						return stream << packet.data.getSubnetworkRequest.subnetworkId;
					} else {
						size_t size = packet.header.packetData;
						stream.write(packet.data.getSubnetworkReply.contactInfo, size);
						return stream;
					}
					
				case TYPE_SUBSCRIBE:
					if (packet.header.isRequest) {
						return stream << packet.data.subscribeRequest.subnetworkId;
					} else {
						return stream;
					}
					
				case TYPE_NODELOOKUP:
					if (packet.header.isRequest) {
						return stream << packet.data.nodeLookupRequest.nodeId;
					} else {
						writeNodes(stream, packet.data.nodeLookupReply.nodeList, packet.header.packetData);
						return stream;
					}
					
				case TYPE_GETSUBSCRIBERS:
					if (packet.header.isRequest) {
						return stream << packet.data.getSubscribersRequest.subnetworkId;
					} else {
						writeNodes(stream, packet.data.getSubscribersReply.subscriberList, packet.header.packetData);
						return stream;
					}
					
				case TYPE_PUSHSUBSCRIBERS:
					if (packet.header.isRequest) {
						writeNodes(stream, packet.data.pushSubscribersRequest.subscriberList, packet.header.packetData);
						return stream;
					} else {
						return stream;
					}
					
					return stream;
					
				default:
					return stream;
			}
		}
		
	}
	
}
