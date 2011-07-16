#include <stdint.h>

#include <OpenP2P/BinaryStream.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>

namespace OpenP2P{

	namespace RootNetwork{
	
		BinaryIStream& operator>>(BinaryIStream& stream, PacketHeader& header){
			uint8_t firstByte = 0;
			BinaryIStream& s = stream >> firstByte >> header.packetData >> header.rpcNumber >> header.destinationId;
			header.version = firstByte >> 4;
			header.isRequest = (firstByte >> 3) & 1;
			header.type = firstByte & 7; //get last 3 bits
			return s;
		}

		BinaryOStream& operator<<(BinaryOStream& stream, const PacketHeader& header){
			uint8_t firstByte = (header.version << 4) | (uint8_t(header.isRequest) << 3) | header.type;
			return stream << firstByte << header.packetData << header.rpcNumber << header.destinationId;
		}
		
		BinaryIStream& operator>>(BinaryIStream& stream, Packet& packet){
			stream >> packet.header;
			switch(packet.header.type){
				case TYPE_PING:
					if(packet.header.isRequest){
						return stream >> packet.data.pingRequest;
					}else{
						return stream >> packet.data.pingReply;
					}
				case TYPE_GETSUBNETWORK:
					if(packet.header.isRequest){
						return stream >> packet.data.getSubnetworkRequest;
					}else{
						return stream >> packet.data.getSubnetworkReply;
					}
				case TYPE_SUBSCRIBE:
					if(packet.header.isRequest){
						return stream >> packet.data.subscribeRequest;
					}else{
						return stream >> packet.data.subscribeReply;
					}
				case TYPE_NODELOOKUP:
					if(packet.header.isRequest){
						return stream >> packet.data.nodeLookupRequest;
					}else{
						return stream >> packet.data.nodeLookupReply;
					}
				case TYPE_GETSUBSCRIBERS:
					if(packet.header.isRequest){
						return stream >> packet.data.getSubscribersRequest;
					}else{
						return stream >> packet.data.getSubscribersReply;
					}
				case TYPE_PUSHSUBSCRIBERS:
					if(packet.header.isRequest){
						return stream >> packet.data.pushSubscribersRequest;
					}else{
						return stream >> packet.data.pushSubscribersReply;
					}
				default:
					return stream;
			}
		}
		
		BinaryOStream& operator<<(BinaryOStream& stream, const Packet& packet){
			stream << packet.header;
			switch(packet.header.type){
				case TYPE_PING:
					if(packet.header.isRequest){
						return stream << packet.data.pingRequest;
					}else{
						return stream << packet.data.pingReply;
					}
				case TYPE_GETSUBNETWORK:
					if(packet.header.isRequest){
						return stream << packet.data.getSubnetworkRequest;
					}else{
						return stream << packet.data.getSubnetworkReply;
					}
				case TYPE_SUBSCRIBE:
					if(packet.header.isRequest){
						return stream << packet.data.subscribeRequest;
					}else{
						return stream << packet.data.subscribeReply;
					}
				case TYPE_NODELOOKUP:
					if(packet.header.isRequest){
						return stream << packet.data.nodeLookupRequest;
					}else{
						return stream << packet.data.nodeLookupReply;
					}
				case TYPE_GETSUBSCRIBERS:
					if(packet.header.isRequest){
						return stream << packet.data.getSubscribersRequest;
					}else{
						return stream << packet.data.getSubscribersReply;
					}
				case TYPE_PUSHSUBSCRIBERS:
					if(packet.header.isRequest){
						return stream << packet.data.pushSubscribersRequest;
					}else{
						return stream << packet.data.pushSubscribersReply;
					}
				default:
					return stream;
			}
		}
	
	}

}
