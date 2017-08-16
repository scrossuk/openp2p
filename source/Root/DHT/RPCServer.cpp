#include <stdio.h>

#include <set>

#include <p2p/Transport/Socket.hpp>

#include <p2p/Event/Source.hpp>
#include <p2p/Event/Wait.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/PrivateIdentity.hpp>
#include <p2p/Root/PublicIdentity.hpp>
#include <p2p/Root/RoutineIdGenerator.hpp>

#include <p2p/Root/DHT/RPCMessage.hpp>
#include <p2p/Root/DHT/RPCServer.hpp>
#include <p2p/Root/DHT/ServerDelegate.hpp>

namespace p2p {

	namespace Root {
	
		namespace DHT {
			
			NodeId readNodeId(const Buffer& payload) {
				BufferIterator iterator(payload);
				BinaryIStream reader(iterator);
				return NodeId::FromReader(reader);
			}
			
			std::pair<NodeId, std::set<Endpoint>> readSubscribe(const Buffer& payload) {
				BufferIterator iterator(payload);
				BinaryIStream reader(iterator);
				
				const auto targetId = NodeId::FromReader(reader);
				
				std::set<Endpoint> endpointSet;
				while (iterator.position() < payload.size()) {
					endpointSet.insert(Endpoint::Read(reader));
				}
				
				return std::make_pair(targetId, std::move(endpointSet));
			}
			
			RPCServer::RPCServer(Socket<NodeId, Message>& socket, ServerDelegate& delegate)
				: socket_(socket), delegate_(delegate) { }
			
			RPCServer::~RPCServer() { }
			
			Event::Source RPCServer::eventSource() const {
				return socket_.eventSource();
			}
			
			bool RPCServer::processRequest() {
				NodeId senderId;
				Message message;
				const bool result = socket_.receive(senderId, message);
				
				if (!result) {
					return false;
				}
				
				if (message.routineState != STATE_0) {
					// Not a request.
					return false;
				}
				
				if (!message.subnetwork || *(message.subnetwork) != NetworkId::Generate("p2p.rootdht")) {
					// Response is not for this subnetwork.
					return false;
				}
				
				switch (message.type) {
					case RPCMessage::GET_NEAREST_NODES: {
						const auto targetId = readNodeId(message.payload);
						auto nodeInfoList = delegate_.getNearestNodes(targetId);
						const auto sendMessage = RPCMessage::GetNearestNodesReply(std::move(nodeInfoList)).createMessage(message.routine);
						socket_.send(senderId, sendMessage);
						return true;
					}
					
					case RPCMessage::SUBSCRIBE: {
						auto subscribePair = readSubscribe(message.payload);
						delegate_.subscribe(subscribePair.first, NodeInfo(senderId, std::move(subscribePair.second)));
						const auto sendMessage = RPCMessage::SubscribeReply().createMessage(message.routine);
						socket_.send(senderId, sendMessage);
						return true;
					}
					
					case RPCMessage::GET_SUBSCRIBERS: {
						const auto targetId = readNodeId(message.payload);
						auto nodeInfoList = delegate_.getSubscribers(targetId);
						const auto sendMessage = RPCMessage::GetSubscribersReply(std::move(nodeInfoList)).createMessage(message.routine);
						socket_.send(senderId, sendMessage);
						return true;
					}
					
					default:
						return false;
				}
			}
			
		}
		
	}
	
}


