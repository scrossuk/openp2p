#ifndef P2P_ROOTNETWORK_CORE_RPCMESSAGE_HPP
#define P2P_ROOTNETWORK_CORE_RPCMESSAGE_HPP

#include <vector>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/NodeId.hpp>

namespace p2p {

	namespace Root {
	
		namespace Core {
		
			class RPCMessage {
				public:
					static RPCMessage IdentifyRequest();
					static RPCMessage IdentifyReply(const Endpoint& endpoint);
					
					static RPCMessage PingRequest();
					static RPCMessage PingReply(const Endpoint& endpoint);
					
					static RPCMessage QueryNetworksRequest();
					static RPCMessage QueryNetworksReply(const std::vector<NetworkId>& networks);
					
					// TODO
					// static RPCMessage KeyExchangeRequest();
					// static RPCMessage KeyExchangeRequest();
					
					enum Kind {
						IDENTIFY = 0,
						PING = 1,
						QUERY_NETWORKS = 4,
						KEY_EXCHANGE = 8
					};
					
					Kind kind() const;
					State state() const;
					const Endpoint& endpoint() const;
					const std::vector<NetworkId>& networks() const;
					
					Buffer toPayload() const;
					
					Message createMessage(uint32_t routine, const NodeId& sourceId, const NodeId& destinationId) const;
					
				private:
					RPCMessage(Kind kind, State state);
					
					Kind kind_;
					State state_;
					Endpoint endpoint_;
					std::vector<NetworkId> networks_;
					
					
			};
			
		}
		
	}
	
}

#endif
