#ifndef P2P_ROOTNETWORK_DHT_RPCFINDSOCKET_HPP
#define P2P_ROOTNETWORK_DHT_RPCFINDSOCKET_HPP

#include <p2p/Socket.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/RoutineIdGenerator.hpp>

namespace p2p {

	namespace Root {
	
		namespace DHT {
		
			class RPCFindSocket: public Kademlia::RPCSocket<NodeId> {
				public:
					RPCFindSocket(Socket<NodeId, Message>& socket, RoutineIdGenerator& routineIdGenerator);
					~RPCFindSocket();
					
					Event::Source eventSource() const;
					
					RPC::Operation<std::vector<NodeId>> performFind(const NodeId& destId, const NodeId& searchId);
					
				private:
					// Non-copyable.
					RPCFindSocket(const RPCFindSocket&) = delete;
					RPCFindSocket& operator=(RPCFindSocket) = delete;
					
					Socket<NodeId, Message>& socket_;
					RoutineIdGenerator& routineIdGenerator_;
					
			};
			
		}
		
	}
	
}

#endif
