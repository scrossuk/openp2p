#ifndef P2P_ROOTNETWORK_ENDPOINTMAPSOCKET_HPP
#define P2P_ROOTNETWORK_ENDPOINTMAPSOCKET_HPP

#include <p2p/Root/Root_api.hpp>
#include <p2p/Transport/Socket.hpp>

#include <p2p/Event/Source.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NodeDatabase.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/NodePair.hpp>

namespace p2p {
	
	namespace Root {
		
		class ROOT_OPENP2P_API EndpointMapSocket: public Socket<NodeId, Message> {
			public:
				EndpointMapSocket(Socket<NodePair, Message>& socket, NodeDatabase& database);
				
				bool isValid() const;
				
				Event::Source eventSource() const;
				
				bool receive(NodeId& nodeId, Message& message);
				
				bool send(const NodeId& nodeId, const Message& message);
				
			private:
				EndpointMapSocket(const EndpointMapSocket&) = delete;
				EndpointMapSocket& operator=(EndpointMapSocket) = delete;
				
				Socket<NodePair, Message>& socket_;
				NodeDatabase& database_;
				
		};
		
	}
	
}

#endif
