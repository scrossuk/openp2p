#include <assert.h>

#include <OpenP2P/Event/Source.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/EndpointMapSocket.hpp>
#include <OpenP2P/RootNetwork/Message.hpp>
#include <OpenP2P/RootNetwork/NodeDatabase.hpp>
#include <OpenP2P/RootNetwork/NodeId.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		EndpointMapSocket::EndpointMapSocket(Socket<Endpoint, Message>& socket, NodeDatabase& database)
			: socket_(socket), database_(database) { }
		
		bool EndpointMapSocket::isValid() const {
			return socket_.isValid();
		}
		
		Event::Source EndpointMapSocket::eventSource() const {
			return socket_.eventSource();
		}
		
		bool EndpointMapSocket::receive(NodeId& nodeId, Message& message) {
			Endpoint endpoint;
			if (!socket_.receive(endpoint, message)) {
				return false;
			}
			
			nodeId = message.sourceId;
			
			if (!database_.isKnownId(nodeId)) {
				return false;
			}
			
			auto& nodeInfo = database_.nodeInfo(nodeId);
			nodeInfo.endpointSet.insert(endpoint);
			
			return true;
		}
		
		bool EndpointMapSocket::send(const NodeId& nodeId, const Message& message) {
			if (!database_.isKnownId(nodeId)) {
				return false;
			}
			
			auto& nodeInfo = database_.nodeInfo(nodeId);
			
			// Broadcast message to all endpoints.
			for (const auto& endpoint: nodeInfo.endpointSet) {
				(void) socket_.send(endpoint, message);
			}
			
			return true;
		}
		
	}
	
}

