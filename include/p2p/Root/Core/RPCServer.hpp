#ifndef P2P_ROOTNETWORK_CORE_RPCSERVER_HPP
#define P2P_ROOTNETWORK_CORE_RPCSERVER_HPP

#include <stdint.h>

#include <vector>

#include <p2p/Socket.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NetworkId.hpp>

namespace p2p {

	namespace Root {
	
		namespace Core {
		
			class RPCServer {
				public:
					RPCServer(Socket<std::pair<Endpoint, NodeId>, Message>& socket);
					~RPCServer();
					
					Event::Source eventSource() const;
					
					void addNetwork(const std::string& networkName);
					
					bool processRequest();
					
				private:
					// Non-copyable.
					RPCServer(const RPCServer&) = delete;
					RPCServer& operator=(RPCServer) = delete;
					
					Socket<std::pair<Endpoint, NodeId>, Message>& socket_;
					std::vector<NetworkId> networks_;
					
			};
			
		}
		
	}
	
}

#endif
