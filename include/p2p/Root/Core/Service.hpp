#ifndef P2P_ROOTNETWORK_CORE_SERVICE_HPP
#define P2P_ROOTNETWORK_CORE_SERVICE_HPP

#include <vector>

#include <p2p/Root/Root_api.hpp>

#include <p2p/Transport/MultiplexSocket.hpp>
#include <p2p/Transport/Socket.hpp>

#include <p2p/Event/Source.hpp>
#include <p2p/Event/UnionGenerator.hpp>

#include <p2p/RPC/Host.hpp>
#include <p2p/RPC/Operation.hpp>

#include <p2p/Root/Message.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/NodePair.hpp>

#include <p2p/Root/Core/RPCClient.hpp>
#include <p2p/Root/Core/RPCServer.hpp>

namespace p2p {

	namespace Root {
	
		namespace Core {
		
			class ROOT_OPENP2P_API Service {
				public:
					Service(Socket<NodePair, Message>& socket, RoutineIdGenerator& routineIdGenerator);
					~Service();
					
					Event::Source eventSource() const;
					
					bool processMessage();
					
					void addNetwork(const std::string& networkName);
					
					RPC::Operation<NodeId> identify(const Endpoint& endpoint);
					
					RPC::Operation<Endpoint> ping(const Endpoint& endpoint, const NodeId& nodeId);
					
					RPC::Operation<std::vector<NetworkId>> queryNetworks(const Endpoint& endpoint, const NodeId& nodeId);
					
				private:
					// Non-copyable.
					Service(const Service&) = delete;
					Service& operator=(Service) = delete;
					
					Socket<NodePair, Message>& socket_;
					MultiplexHost<NodePair, Message> multiplexHost_;
					MultiplexClient<NodePair, Message> clientSocket_;
					MultiplexClient<NodePair, Message> serverSocket_;
					RPCClient client_;
					RPCServer server_;
					Event::UnionGenerator unionGenerator_;
					
			};
			
		}
		
	}
	
}

#endif
