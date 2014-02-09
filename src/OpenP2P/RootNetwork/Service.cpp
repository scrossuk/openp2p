#include <stdio.h>

#include <queue>

#include <OpenP2P/Socket.hpp>

#include <OpenP2P/Event/Source.hpp>
#include <OpenP2P/Event/Wait.hpp>

#include <OpenP2P/RootNetwork/CoreMessage.hpp>
#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/NodeId.hpp>
#include <OpenP2P/RootNetwork/PrivateIdentity.hpp>
#include <OpenP2P/RootNetwork/PublicIdentity.hpp>
#include <OpenP2P/RootNetwork/Service.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		Service::Service(Socket<Endpoint, Packet>& socket, PrivateIdentity& identity)
			: socket_(socket), identity_(identity), nextRoutine_(0) { }
		
		Service::~Service() { }
		
		NodeId Service::identifyEndpoint(const Endpoint& endpoint) {
			NodeId destination;
			destination.fill(0x00);
			
			const uint32_t routineId = nextRoutine_++;
			socket_.send(endpoint, CoreMessage::IdentifyRequest().createPacket(routineId, identity_.nextPacketCount(), destination));
			
			while (true) {
				Endpoint receiveEndpoint;
				Packet receivePacket;
				const bool result = socket_.receive(receiveEndpoint, receivePacket);
				if (!result) {
					Event::Wait(socket_.eventSource());
					continue;
				}
				
				if (receivePacket.header.routine != routineId) {
					packetQueue_.push(receivePacket);
					continue;
				}
				
				printf("Got reply.\n");
				
				// TODO.
				NodeId sourceId;
				sourceId.fill(0x00);
				return sourceId;
			}
		}
		
		void Service::processRequests() {
			while (true) {
				Endpoint receiveEndpoint;
				Packet receivePacket;
				const bool result = socket_.receive(receiveEndpoint, receivePacket);
				if (!result) {
					Event::Wait(socket_.eventSource());
					continue;
				}
				
				if (receivePacket.header.type != CoreMessage::IDENTIFY) {
					printf("Unknown type.\n");
					continue;
				}
				
				// TODO.
				NodeId destination;
				destination.fill(0x00);
				
				printf("Handling IDENTIFY request.\n");
				
				const auto sendPacket = CoreMessage::IdentifyReply(receiveEndpoint).createPacket(
					receivePacket.header.routine, identity_.nextPacketCount(), destination);
				socket_.send(receiveEndpoint, sendPacket);
			}
		}
		
	}
	
}

