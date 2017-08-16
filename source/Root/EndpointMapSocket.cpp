#include <assert.h>

#include <p2p/Event/Source.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/EndpointMapSocket.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NodeDatabase.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/NodePair.hpp>

namespace p2p {

	namespace Root {
	
		EndpointMapSocket::EndpointMapSocket(Socket<NodePair, Message>& socket, NodeDatabase& database)
			: socket_(socket), database_(database) { }
		
		bool EndpointMapSocket::isValid() const {
			return socket_.isValid();
		}
		
		Event::Source EndpointMapSocket::eventSource() const {
			return socket_.eventSource();
		}
		
		bool EndpointMapSocket::receive(NodeId& nodeId, Message& message) {
			NodePair nodePair;
			if (!socket_.receive(nodePair, message)) {
				return false;
			}
			
			nodeId = nodePair.id;
			
			if (!database_.isKnownId(nodeId)) {
				return false;
			}
			
			auto& nodeEntry = database_.nodeEntry(nodeId);
			nodeEntry.endpointSet.insert(nodePair.endpoint);
			
			return true;
		}
		
		bool EndpointMapSocket::send(const NodeId& nodeId, const Message& message) {
			if (!database_.isKnownId(nodeId)) {
				return false;
			}
			
			const auto& nodeEntry = database_.nodeEntry(nodeId);
			const auto destinationId = nodeEntry.identity.id();
			
			// Broadcast message to all endpoints.
			for (const auto& endpoint: nodeEntry.endpointSet) {
				(void) socket_.send(NodePair(destinationId, endpoint), message);
			}
			
			return true;
		}
		
	}
	
}

