#ifndef P2P_ROOTNETWORK_DHT_RPCSERVER_HPP
#define P2P_ROOTNETWORK_DHT_RPCSERVER_HPP

#include <p2p/Socket.hpp>

#include <p2p/Root/Message.hpp>
#include <p2p/Root/NodeId.hpp>

namespace p2p {

	namespace Root {
	
		namespace DHT {
		
			class RPCServer {
				public:
					RPCServer(Socket<NodeId, Message>& socket);
					~RPCServer();
					
				private:
					// Non-copyable.
					RPCServer(const RPCServer&) = delete;
					RPCServer& operator=(RPCServer) = delete;
					
					Socket<NodeId, Message>& socket_;
					
			};
			
		}
		
	}
	
}

#endif
