#include <stdio.h>

#include <queue>

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

#include <p2p/Root/DHT/RPCFindSocket.hpp>
#include <p2p/Root/DHT/RPCMessage.hpp>

namespace p2p {

	namespace Root {
	
		namespace DHT {
		
			RPCFindSocket::RPCFindSocket(Socket<NodeId, Message>& socket, RoutineIdGenerator& routineIdGenerator)
				: socket_(socket), routineIdGenerator_(routineIdGenerator) { }
				
			RPCFindSocket::~RPCFindSocket() { }
			
			Event::Source RPCFindSocket::eventSource() const {
				return socket_.eventSource();
			}

			RPC::Operation<std::vector<NodeId>> RPCFindSocket::performFind(const NodeId& destId, const NodeId& searchId){
				sendFind(destId, searchId);
			}
			
			void RPCFindSocket::sendFind(const NodeId& destId, const NodeId& searchId) {
				const auto routineId = routineIdGenerator_.generateId();
				socket_.send(destId, RPCMessage::GetNearestNodesRequest(searchId).createMessage(routineId));
			}
			
			bool RPCFindSocket::receiveFind(NodeId& sourceId, std::vector<NodeId>& group) {
				Message message;
				const bool result = socket_.receive(sourceId, message);
				if (!result) return false;
				
				if (!message.subnetwork || *(message.subnetwork) != NetworkId::Generate("p2p.rootdht")) {
					return false;
				}
			}
			
		}
		
	}
	
}


