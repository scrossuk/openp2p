#include <stdio.h>

#include <queue>

#include <p2p/Socket.hpp>

#include <p2p/Event/Source.hpp>
#include <p2p/Event/Wait.hpp>

#include <p2p/Root/CoreMessage.hpp>
#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/PrivateIdentity.hpp>
#include <p2p/Root/PublicIdentity.hpp>
#include <p2p/Root/RoutineIdGenerator.hpp>
#include <p2p/Root/DHTRPCService.hpp>

namespace p2p {

	namespace Root {
	
		DHTRPCService::DHTRPCService(Socket<Endpoint, Message>& socket, RoutineIdGenerator& routineIdGenerator)
			: socket_(socket), routineIdGenerator_(routineIdGenerator) { }
		
		DHTRPCService::~DHTRPCService() { }
		
		void DHTRPCService::addNetwork(const std::string& networkName) {
			networks_.push_back(NetworkId::Generate(networkName));
		}
		
		NodeId DHTRPCService::identifyEndpoint(const Endpoint& endpoint) {
			// Destination isn't known for IDENTIFY messages,
			// so the ID field is zeroed.
			const auto destinationId = NodeId::Zero();
			
			const auto routineId = routineIdGenerator_.generateId();
			socket_.send(endpoint, CoreMessage::IdentifyRequest().createMessage(routineId, NodeId::Zero(), destinationId));
			
			while (true) {
				Endpoint receiveEndpoint;
				Message receiveMessage;
				const bool result = socket_.receive(receiveEndpoint, receiveMessage);
				if (!result) {
					Event::Wait(socket_.eventSource());
					continue;
				}
				
				if (receiveMessage.routine != routineId ||
					receiveMessage.routineState != STATE_1 ||
					receiveMessage.type != CoreMessage::IDENTIFY) {
					continue;
				}
				
				printf("Got IDENTIFY reply.\n");
				
				return receiveMessage.sourceId;
			}
		}
		
		Endpoint DHTRPCService::pingNode(const Endpoint& endpoint, const NodeId& nodeId) {
			const auto routineId = routineIdGenerator_.generateId();
			socket_.send(endpoint, CoreMessage::PingRequest().createMessage(routineId, NodeId::Zero(), nodeId));
			
			while (true) {
				Endpoint receiveEndpoint;
				Message receiveMessage;
				const bool result = socket_.receive(receiveEndpoint, receiveMessage);
				if (!result) {
					Event::Wait(socket_.eventSource());
					continue;
				}
				
				if (receiveMessage.routine != routineId ||
					receiveMessage.routineState != STATE_1 ||
					receiveMessage.type != CoreMessage::PING) {
					continue;
				}
				
				printf("Got PING reply.\n");
				
				BufferIterator iterator(receiveMessage.payload);
				BinaryIStream blockingReader(iterator);
				
				return Endpoint::Read(blockingReader);
			}
		}
		
		std::vector<NetworkId> DHTRPCService::queryNetworks(const Endpoint& endpoint, const NodeId& nodeId) {
			const auto routineId = routineIdGenerator_.generateId();
			socket_.send(endpoint, CoreMessage::QueryNetworksRequest().createMessage(routineId, NodeId::Zero(), nodeId));
			
			while (true) {
				Endpoint receiveEndpoint;
				Message receiveMessage;
				const bool result = socket_.receive(receiveEndpoint, receiveMessage);
				if (!result) {
					Event::Wait(socket_.eventSource());
					continue;
				}
				
				if (receiveMessage.routine != routineId ||
					receiveMessage.routineState != STATE_1 ||
					receiveMessage.type != CoreMessage::QUERY_NETWORKS) {
					continue;
				}
				
				printf("Got QUERY_SUBNETWORKS reply.\n");
				
				BufferIterator iterator(receiveMessage.payload);
				BinaryIStream blockingReader(iterator);
				
				const size_t networkCount = receiveMessage.payload.size() / NETWORK_ID_SIZE_BYTES;
				
				std::vector<NetworkId> networks;
				
				for (size_t i = 0; i < networkCount; i++) {
					networks.push_back(NetworkId::FromReader(blockingReader));
				}
				
				return networks;
			}
		}
		
		void DHTRPCService::processRequests() {
			printf("Processing requests.\n");
			
			while (true) {
				Endpoint receiveEndpoint;
				Message receiveMessage;
				const bool result = socket_.receive(receiveEndpoint, receiveMessage);
				if (!result) {
					Event::Wait(socket_.eventSource());
					continue;
				}
				
				if (receiveMessage.routineState != STATE_0) {
					// Not a request.
					continue;
				}
				
				if (receiveMessage.subnetwork) {
					// Request is for a subnetwork.
					continue;
				}
				
				switch (receiveMessage.type) {
					case CoreMessage::IDENTIFY: {
						const auto& senderId = receiveMessage.destinationId;
						
						printf("Handling IDENTIFY request.\n");
						
						const auto sendMessage = CoreMessage::IdentifyReply(receiveEndpoint).createMessage(
							receiveMessage.routine, NodeId::Zero(), senderId);
						socket_.send(receiveEndpoint, sendMessage);
						break;
					}
					case CoreMessage::PING: {
						const auto& senderId = receiveMessage.destinationId;
						
						printf("Handling PING request.\n");
						
						const auto sendMessage = CoreMessage::PingReply(receiveEndpoint).createMessage(
							receiveMessage.routine, NodeId::Zero(), senderId);
						socket_.send(receiveEndpoint, sendMessage);
						break;
					}
					case CoreMessage::QUERY_NETWORKS: {
						const auto& senderId = receiveMessage.destinationId;
						
						printf("Handling QUERY_SUBNETWORKS request.\n");
						
						const auto sendMessage = CoreMessage::QueryNetworksReply(networks_).createMessage(
							receiveMessage.routine, NodeId::Zero(), senderId);
						socket_.send(receiveEndpoint, sendMessage);
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

