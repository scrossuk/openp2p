#ifndef P2P_ROOTNETWORK_DHT_SERVICE_HPP
#define P2P_ROOTNETWORK_DHT_SERVICE_HPP

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

#include <p2p/Root/DHT/RPCClient.hpp>
#include <p2p/Root/DHT/RPCServer.hpp>
#include <p2p/Root/DHT/ServerDelegate.hpp>

namespace p2p {

	namespace Root {
	
		namespace DHT {
		
			class ROOT_OPENP2P_API Service {
				public:
					Service(Socket<NodeId, Message>& socket, RoutineIdGenerator& routineIdGenerator, ServerDelegate& delegate);
					~Service();
					
					Event::Source eventSource() const;
					
					bool processMessage();
					
					RPC::Operation<std::vector<NodeInfo>> getNearestNodes(const NodeId& destId, const NodeId& targetId);
					
					RPC::Operation<Empty> subscribe(const NodeId& destId, const NodeId& targetId, const std::vector<Endpoint>& myEndpoints);
					
					RPC::Operation<std::vector<NodeInfo>> getSubscribers(const NodeId& destId, const NodeId& targetId);
					
				private:
					// Non-copyable.
					Service(const Service&) = delete;
					Service& operator=(Service) = delete;
					
					Socket<NodeId, Message>& socket_;
					MultiplexHost<Root::NodeId, Root::Message> multiplexHost_;
					MultiplexClient<Root::NodeId, Root::Message> clientSocket_;
					MultiplexClient<Root::NodeId, Root::Message> serverSocket_;
					RPCClient client_;
					RPCServer server_;
					Event::UnionGenerator unionGenerator_;
					
			};
			
		}
		
	}
	
}

#endif
