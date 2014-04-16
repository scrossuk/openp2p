#ifndef OPENP2P_ROOTNETWORK_ENDPOINTMAPSOCKET_HPP
#define OPENP2P_ROOTNETWORK_ENDPOINTMAPSOCKET_HPP

#include <OpenP2P/Socket.hpp>

#include <OpenP2P/Event/Source.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Message.hpp>
#include <OpenP2P/RootNetwork/NodeDatabase.hpp>
#include <OpenP2P/RootNetwork/NodeId.hpp>

namespace OpenP2P {
	
	namespace RootNetwork {
		
		class EndpointMapSocket: public Socket<NodeId, Message> {
			public:
				EndpointMapSocket(Socket<Endpoint, Message>& socket, NodeDatabase& database);
				
				bool isValid() const;
				
				Event::Source eventSource() const;
				
				bool receive(NodeId& nodeId, Message& message);
				
				bool send(const NodeId& nodeId, const Message& message);
				
			private:
				EndpointMapSocket(const EndpointMapSocket&) = delete;
				EndpointMapSocket& operator=(EndpointMapSocket) = delete;
				
				Socket<Endpoint, Message>& socket_;
				NodeDatabase& database_;
				
		};
		
	}
	
}

#endif
