#include <assert.h>

#include <p2p/Event/Source.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/EndpointMapSocket.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NodeDatabase.hpp>
#include <p2p/Root/NodeId.hpp>

namespace p2p {

	namespace Root {
	
		EndpointMapSocket::EndpointMapSocket(Socket<std::pair<Endpoint, NodeId>, Message>& socket, NodeDatabase& database)
			: socket_(socket), database_(database) { }
		
		bool EndpointMapSocket::isValid() const {
			return socket_.isValid();
		}
		
		Event::Source EndpointMapSocket::eventSource() const {
			return socket_.eventSource();
		}
		
		bool EndpointMapSocket::receive(NodeId& nodeId, Message& message) {
			std::pair<Endpoint, NodeId> endpoint;
			if (!socket_.receive(endpoint, message)) {
				return false;
			}
			
			nodeId = endpoint.second;
			
			if (!database_.isKnownId(nodeId)) {
				return false;
			}
			
			auto& nodeInfo = database_.nodeInfo(nodeId);
			nodeInfo.endpointSet.insert(endpoint.first);
			
			return true;
		}
		
		bool EndpointMapSocket::send(const NodeId& nodeId, const Message& message) {
			if (!database_.isKnownId(nodeId)) {
				return false;
			}
			
			const auto& nodeInfo = database_.nodeInfo(nodeId);
			const auto destinationId = nodeInfo.identity.id();
			
			// Broadcast message to all endpoints.
			for (const auto& endpoint: nodeInfo.endpointSet) {
				(void) socket_.send(std::make_pair(endpoint, destinationId), message);
			}
			
			return true;
		}
		
	}
	
}

