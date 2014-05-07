#include <stdio.h>

#include <p2p/Transport/Socket.hpp>

#include <p2p/Event/Source.hpp>
#include <p2p/Event/Wait.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/NodePair.hpp>
#include <p2p/Root/PrivateIdentity.hpp>
#include <p2p/Root/PublicIdentity.hpp>
#include <p2p/Root/RoutineIdGenerator.hpp>

#include <p2p/Root/Core/RPCMessage.hpp>
#include <p2p/Root/Core/RPCClient.hpp>

namespace p2p {

	namespace Root {
	
		namespace Core {
		
			RPCClient::RPCClient(Socket<NodePair, Message>& socket, RoutineIdGenerator& routineIdGenerator, double timeoutMilliseconds)
				: socket_(socket), resender_(socket_, timeoutMilliseconds),
				unionGenerator_({ socket_.eventSource(), resender_.eventSource() }),
				routineIdGenerator_(routineIdGenerator) { }
			
			RPCClient::~RPCClient() { }
			
			Event::Source RPCClient::eventSource() const {
				return unionGenerator_.eventSource();
			}
			
			bool RPCClient::processResponse() {
				resender_.processResends();
				
				NodePair nodePair;
				Message message;
				const bool result = socket_.receive(nodePair, message);
				
				if (!result) {
					return false;
				}
				
				if (message.routineState != STATE_1) {
					// Not a response.
					return false;
				}
				
				if (message.subnetwork) {
					// Response is for a subnetwork.
					return false;
				}
				
				switch (message.type) {
					case RPCMessage::IDENTIFY: {
						resender_.endRoutine(message.routine);
						identifyHost_.completeOperation(message.routine, nodePair.id);
						return true;
					}
					
					case RPCMessage::PING: {
						BufferIterator iterator(message.payload);
						BinaryIStream blockingReader(iterator);
						
						const auto myEndpoint = Endpoint::Read(blockingReader);
						resender_.endRoutine(message.routine);
						pingHost_.completeOperation(message.routine, myEndpoint);
						return true;
					}
					
					case RPCMessage::QUERY_NETWORKS: {
						BufferIterator iterator(message.payload);
						BinaryIStream blockingReader(iterator);
						
						const size_t networkCount = message.payload.size() / NETWORK_ID_SIZE_BYTES;
						
						std::vector<NetworkId> networks;
						
						for (size_t i = 0; i < networkCount; i++) {
							networks.push_back(NetworkId::FromReader(blockingReader));
						}
						
						resender_.endRoutine(message.routine);
						queryNetworksHost_.completeOperation(message.routine, std::move(networks));
						return true;
					}
					
					default:
						return false;
				}
			}
			
			RPC::Operation<NodeId> RPCClient::identify(const Endpoint& endpoint) {
				// Destination isn't known for IDENTIFY messages,
				// so the ID field is zeroed.
				const auto nodeId = NodeId::Zero();
				
				const auto routineId = routineIdGenerator_.generateId();
				resender_.startRoutine(routineId, NodePair(nodeId, endpoint), RPCMessage::IdentifyRequest().createMessage(routineId));
				return identifyHost_.startOperation(routineId);
			}
			
			RPC::Operation<Endpoint> RPCClient::ping(const Endpoint& endpoint, const NodeId& nodeId) {
				const auto routineId = routineIdGenerator_.generateId();
				resender_.startRoutine(routineId, NodePair(nodeId, endpoint), RPCMessage::PingRequest().createMessage(routineId));
				return pingHost_.startOperation(routineId);
			}
			
			RPC::Operation<std::vector<NetworkId>> RPCClient::queryNetworks(const Endpoint& endpoint, const NodeId& nodeId) {
				const auto routineId = routineIdGenerator_.generateId();
				resender_.startRoutine(routineId, NodePair(nodeId, endpoint), RPCMessage::QueryNetworksRequest().createMessage(routineId));
				return queryNetworksHost_.startOperation(routineId);
			}
			
		}
		
	}
	
}

