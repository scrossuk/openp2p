#include <stdio.h>

#include <vector>

#include <p2p/Transport/Socket.hpp>

#include <p2p/Event/Source.hpp>
#include <p2p/Event/Wait.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/NodePair.hpp>

#include <p2p/Root/Core/RPCMessage.hpp>
#include <p2p/Root/Core/RPCServer.hpp>

namespace p2p {

	namespace Root {
	
		namespace Core {
		
			RPCServer::RPCServer(Socket<NodePair, Message>& socket)
				: socket_(socket) { }
				
			RPCServer::~RPCServer() { }
			
			Event::Source RPCServer::eventSource() const {
				return socket_.eventSource();
			}
			
			void RPCServer::addNetwork(const std::string& networkName) {
				networks_.push_back(NetworkId::Generate(networkName));
			}
			
			bool RPCServer::processRequest() {
				NodePair nodePair;
				Message message;
				const bool result = socket_.receive(nodePair, message);
				
				if (!result) {
					return false;
				}
				
				if (message.routineState != STATE_0) {
					// Not a request.
					return false;
				}
				
				if (message.subnetwork) {
					// Request is for a subnetwork.
					return false;
				}
				
				switch (message.type) {
					case RPCMessage::IDENTIFY: {
						const auto sendMessage = RPCMessage::IdentifyReply(nodePair.endpoint).createMessage(message.routine);
						socket_.send(nodePair, sendMessage);
						return true;
					}
					
					case RPCMessage::PING: {
						const auto sendMessage = RPCMessage::PingReply(nodePair.endpoint).createMessage(message.routine);
						socket_.send(nodePair, sendMessage);
						return true;
					}
					
					case RPCMessage::QUERY_NETWORKS: {
						const auto sendMessage = RPCMessage::QueryNetworksReply(networks_).createMessage(message.routine);
						socket_.send(nodePair, sendMessage);
						return true;
					}
					
					default:
						return false;
				}
			}
			
		}
		
	}
	
}

