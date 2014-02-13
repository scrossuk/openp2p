#include <stdio.h>

#include <queue>

#include <OpenP2P/Socket.hpp>

#include <OpenP2P/Event/Source.hpp>
#include <OpenP2P/Event/Wait.hpp>

#include <OpenP2P/RootNetwork/CoreMessage.hpp>
#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/NetworkId.hpp>
#include <OpenP2P/RootNetwork/NodeId.hpp>
#include <OpenP2P/RootNetwork/PrivateIdentity.hpp>
#include <OpenP2P/RootNetwork/PublicIdentity.hpp>
#include <OpenP2P/RootNetwork/Service.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		Service::Service(Socket<Endpoint, Packet>& socket)
			: socket_(socket), nextRoutine_(0) { }
		
		Service::~Service() { }
		
		void Service::addNetwork(const std::string& networkName) {
			networks_.push_back(NetworkId::Generate(networkName));
		}
		
		NodeId Service::identifyEndpoint(const Endpoint& endpoint) {
			// Destination isn't known for IDENTIFY messages,
			// so the ID field is zeroed.
			const auto destinationId = NodeId::Zero();
			
			const uint32_t routineId = nextRoutine_++;
			socket_.send(endpoint, CoreMessage::IdentifyRequest().createPacket(routineId, destinationId));
			
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
				
				printf("Got IDENTIFY reply.\n");
				
				BufferIterator iterator(receivePacket.payload);
				BinaryIStream blockingReader(iterator);
				
				// TODO.
				return NodeId::Zero();
			}
		}
		
		Endpoint Service::pingNode(const Endpoint& endpoint, const NodeId& nodeId) {
			const uint32_t routineId = nextRoutine_++;
			socket_.send(endpoint, CoreMessage::PingRequest().createPacket(routineId, nodeId));
			
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
				
				printf("Got PING reply.\n");
				
				BufferIterator iterator(receivePacket.payload);
				BinaryIStream blockingReader(iterator);
				
				return Endpoint::Read(blockingReader);
			}
		}
		
		std::vector<NetworkId> Service::queryNetworks(const Endpoint& endpoint, const NodeId& nodeId) {
			const uint32_t routineId = nextRoutine_++;
			socket_.send(endpoint, CoreMessage::QueryNetworksRequest().createPacket(routineId, nodeId));
			
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
				
				printf("Got QUERY_SUBNETWORKS reply.\n");
				
				BufferIterator iterator(receivePacket.payload);
				BinaryIStream blockingReader(iterator);
				
				const size_t networkCount = receivePacket.payload.size() / NETWORK_ID_SIZE_BYTES;
				
				std::vector<NetworkId> networks;
				
				for (size_t i = 0; i < networkCount; i++) {
					networks.push_back(NetworkId::FromReader(blockingReader));
				}
				
				return networks;
			}
		}
		
		void Service::processRequests() {
			printf("Processing requests.\n");
			
			while (true) {
				Endpoint receiveEndpoint;
				Packet receivePacket;
				const bool result = socket_.receive(receiveEndpoint, receivePacket);
				if (!result) {
					Event::Wait(socket_.eventSource());
					continue;
				}
				
				if (receivePacket.header.state != STATE_0) {
					printf("Not a request.\n");
					continue;
				}
				
				switch (receivePacket.header.type) {
					case CoreMessage::IDENTIFY: {
						const auto& senderId = receivePacket.header.destinationId;
						
						printf("Handling IDENTIFY request.\n");
						
						const auto sendPacket = CoreMessage::IdentifyReply(receiveEndpoint).createPacket(
							receivePacket.header.routine, senderId);
						socket_.send(receiveEndpoint, sendPacket);
						break;
					}
					case CoreMessage::PING: {
						const auto& senderId = receivePacket.header.destinationId;
						
						printf("Handling PING request.\n");
						
						const auto sendPacket = CoreMessage::PingReply(receiveEndpoint).createPacket(
							receivePacket.header.routine, senderId);
						socket_.send(receiveEndpoint, sendPacket);
						break;
					}
					case CoreMessage::QUERY_NETWORKS: {
						const auto& senderId = receivePacket.header.destinationId;
						
						printf("Handling QUERY_SUBNETWORKS request.\n");
						
						const auto sendPacket = CoreMessage::QueryNetworksReply(networks_).createPacket(
							receivePacket.header.routine, senderId);
						socket_.send(receiveEndpoint, sendPacket);
						break;
					}
					default: {
						printf("Unknown type.\n");
						continue;
					}
				}
			}
		}
		
	}
	
}

