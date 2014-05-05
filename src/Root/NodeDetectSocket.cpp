#include <p2p/Event/Source.hpp>

#include <p2p/Root/Message.hpp>
#include <p2p/Root/NodeDetectSocket.hpp>
#include <p2p/Root/NodePair.hpp>

namespace p2p {

	namespace Root {
	
		NodeDetectSocket::NodeDetectSocket(Socket<NodePair, Message>& socket, NodeDetectDelegate& delegate)
			: socket_(socket), delegate_(delegate) { }
		
		bool NodeDetectSocket::isValid() const {
			return socket_.isValid();
		}
		
		Event::Source NodeDetectSocket::eventSource() const {
			return socket_.eventSource();
		}
		
		bool NodeDetectSocket::receive(NodePair& nodePair, Message& message) {
			if (!socket_.receive(nodePair, message)) {
				return false;
			}
			
			delegate_.detectedNodePair(nodePair);
			
			return true;
		}
		
		bool NodeDetectSocket::send(const NodePair& nodePair, const Message& message) {
			return socket_.send(nodePair, message);
		}
		
	}
	
}

