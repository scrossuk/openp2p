#ifndef P2P_ROOTNETWORK_NODEDETECTSOCKET_HPP
#define P2P_ROOTNETWORK_NODEDETECTSOCKET_HPP

#include <p2p/Root/Root_api.hpp>
#include <p2p/Transport/Socket.hpp>

#include <p2p/Event/Source.hpp>

#include <p2p/Root/Message.hpp>
#include <p2p/Root/NodePair.hpp>

namespace p2p {
	
	namespace Root {
		
		class ROOT_OPENP2P_API NodeDetectDelegate {
			public:
				virtual void detectedNodePair(const NodePair& nodePair) = 0;
				
		};
		
		class ROOT_OPENP2P_API NodeDetectSocket: public Socket<NodePair, Message> {
			public:
				NodeDetectSocket(Socket<NodePair, Message>& socket, NodeDetectDelegate& delegate);
				
				bool isValid() const;
				
				Event::Source eventSource() const;
				
				bool receive(NodePair& nodePair, Message& message);
				
				bool send(const NodePair& nodePair, const Message& message);
				
			private:
				NodeDetectSocket(const NodeDetectSocket&) = delete;
				NodeDetectSocket& operator=(NodeDetectSocket) = delete;
				
				Socket<NodePair, Message>& socket_;
				NodeDetectDelegate& delegate_;
				
		};
		
	}
	
}

#endif
