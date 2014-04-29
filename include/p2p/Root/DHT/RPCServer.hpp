#ifndef P2P_ROOTNETWORK_DHT_RPCSERVER_HPP
#define P2P_ROOTNETWORK_DHT_RPCSERVER_HPP

#include <p2p/Socket.hpp>

#include <p2p/Event/Source.hpp>

#include <p2p/Root/Message.hpp>
#include <p2p/Root/NodeId.hpp>

#include <p2p/Root/DHT/ServerDelegate.hpp>

namespace p2p {

	namespace Root {
	
		namespace DHT {
		
			class RPCServer {
				public:
					RPCServer(Socket<NodeId, Message>& socket, ServerDelegate& delegate);
					~RPCServer();
					
					Event::Source eventSource() const;
					
					bool processRequest();
					
				private:
					// Non-copyable.
					RPCServer(const RPCServer&) = delete;
					RPCServer& operator=(RPCServer) = delete;
					
					Socket<NodeId, Message>& socket_;
					ServerDelegate& delegate_;
					
			};
			
		}
		
	}
	
}

#endif
