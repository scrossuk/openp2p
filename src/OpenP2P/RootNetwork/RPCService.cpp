#include <stdio.h>

#include <queue>

#include <OpenP2P/Socket.hpp>

#include <OpenP2P/Event/Source.hpp>
#include <OpenP2P/Event/Wait.hpp>

#include <OpenP2P/RootNetwork/CoreMessage.hpp>
#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Message.hpp>
#include <OpenP2P/RootNetwork/NetworkId.hpp>
#include <OpenP2P/RootNetwork/NodeId.hpp>
#include <OpenP2P/RootNetwork/PrivateIdentity.hpp>
#include <OpenP2P/RootNetwork/PublicIdentity.hpp>
#include <OpenP2P/RootNetwork/RoutineIdGenerator.hpp>
#include <OpenP2P/RootNetwork/RPCService.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		RPCService::RPCService(Socket<Endpoint, Message>& socket, RoutineIdGenerator& routineIdGenerator)
			: socket_(socket), routineIdGenerator_(routineIdGenerator) { }
		
		RPCService::~RPCService() { }
		
		void RPCService::addNetwork(const std::string& networkName) {
			networks_.push_back(NetworkId::Generate(networkName));
		}
		
		NodeId RPCService::identifyEndpoint(const Endpoint& endpoint) {
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
		
		Endpoint RPCService::pingNode(const Endpoint& endpoint, const NodeId& nodeId) {
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
		
		std::vector<NetworkId> RPCService::queryNetworks(const Endpoint& endpoint, const NodeId& nodeId) {
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
		
		void RPCService::processRequests() {
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

