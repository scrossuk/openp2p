#include <stdio.h>

#include <queue>

#include <p2p/Transport/Socket.hpp>

#include <p2p/Event/Source.hpp>
#include <p2p/Event/UnionGenerator.hpp>
#include <p2p/Event/Wait.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/PrivateIdentity.hpp>
#include <p2p/Root/PublicIdentity.hpp>
#include <p2p/Root/RoutineIdGenerator.hpp>

#include <p2p/Root/DHT/RPCMessage.hpp>
#include <p2p/Root/DHT/Service.hpp>
#include <p2p/Root/DHT/ServerDelegate.hpp>

namespace p2p {

	namespace Root {
	
		namespace DHT {
			
			Service::Service(Socket<NodeId, Message>& socket, RoutineIdGenerator& routineIdGenerator, ServerDelegate& delegate)
				: socket_(socket), multiplexHost_(socket),
				clientSocket_(multiplexHost_),
				serverSocket_(multiplexHost_),
				client_(clientSocket_, routineIdGenerator),
				server_(serverSocket_, delegate),
				unionGenerator_({ client_.eventSource(), server_.eventSource() }) { }
			
			Service::~Service() { }
			
			Event::Source Service::eventSource() const {
				return unionGenerator_.eventSource();
			}
			
			bool Service::processMessage() {
				return client_.processResponse() || server_.processRequest();
			}
			
			RPC::Operation<std::vector<NodeInfo>> Service::getNearestNodes(const NodeId& destId, const NodeId& targetId) {
				return client_.getNearestNodes(destId, targetId);
			}
			
			RPC::Operation<Empty> Service::subscribe(const NodeId& destId, const NodeId& targetId, const std::vector<Endpoint>& myEndpoints) {
				return client_.subscribe(destId, targetId, myEndpoints);
			}
			
			RPC::Operation<std::vector<NodeInfo>> Service::getSubscribers(const NodeId& destId, const NodeId& targetId) {
				return client_.getSubscribers(destId, targetId);
			}
			
		}
		
	}
	
}


