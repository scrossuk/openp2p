#include <stdio.h>

#include <queue>

#include <p2p/Socket.hpp>

#include <p2p/Event/Source.hpp>
#include <p2p/Event/Wait.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/PrivateIdentity.hpp>
#include <p2p/Root/PublicIdentity.hpp>
#include <p2p/Root/RoutineIdGenerator.hpp>

#include <p2p/Root/Core/RPCMessage.hpp>
#include <p2p/Root/Core/RPCService.hpp>

namespace p2p {

	namespace Root {
	
		namespace Core {
		
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
				socket_.send(endpoint, RPCMessage::IdentifyRequest().createMessage(routineId, NodeId::Zero(), destinationId));
				
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
						receiveMessage.type != RPCMessage::IDENTIFY) {
						continue;
					}
					
					printf("Got IDENTIFY reply.\n");
					
					return receiveMessage.sourceId;
				}
			}
			
			Endpoint RPCService::pingNode(const Endpoint& endpoint, const NodeId& nodeId) {
				const auto routineId = routineIdGenerator_.generateId();
				socket_.send(endpoint, RPCMessage::PingRequest().createMessage(routineId, NodeId::Zero(), nodeId));
				
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
						receiveMessage.type != RPCMessage::PING) {
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
				socket_.send(endpoint, RPCMessage::QueryNetworksRequest().createMessage(routineId, NodeId::Zero(), nodeId));
				
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
						receiveMessage.type != RPCMessage::QUERY_NETWORKS) {
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
						case RPCMessage::IDENTIFY: {
							const auto& senderId = receiveMessage.destinationId;
							
							printf("Handling IDENTIFY request.\n");
							
							const auto sendMessage = RPCMessage::IdentifyReply(receiveEndpoint).createMessage(
														 receiveMessage.routine, NodeId::Zero(), senderId);
							socket_.send(receiveEndpoint, sendMessage);
							break;
						}
						
						case RPCMessage::PING: {
							const auto& senderId = receiveMessage.destinationId;
							
							printf("Handling PING request.\n");
							
							const auto sendMessage = RPCMessage::PingReply(receiveEndpoint).createMessage(
														 receiveMessage.routine, NodeId::Zero(), senderId);
							socket_.send(receiveEndpoint, sendMessage);
							break;
						}
						
						case RPCMessage::QUERY_NETWORKS: {
							const auto& senderId = receiveMessage.destinationId;
							
							printf("Handling QUERY_SUBNETWORKS request.\n");
							
							const auto sendMessage = RPCMessage::QueryNetworksReply(networks_).createMessage(
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
	
}

