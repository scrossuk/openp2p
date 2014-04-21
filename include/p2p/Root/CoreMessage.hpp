#ifndef P2P_ROOTNETWORK_COREMESSAGE_HPP
#define P2P_ROOTNETWORK_COREMESSAGE_HPP

#include <vector>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/NodeId.hpp>

namespace p2p {

	namespace Root {
	
		class CoreMessage {
			public:
				static CoreMessage IdentifyRequest();
				static CoreMessage IdentifyReply(const Endpoint& endpoint);
				
				static CoreMessage PingRequest();
				static CoreMessage PingReply(const Endpoint& endpoint);
				
				static CoreMessage QueryNetworksRequest();
				static CoreMessage QueryNetworksReply(const std::vector<NetworkId>& networks);
				
				// TODO
				// static CoreMessage KeyExchangeRequest();
				// static CoreMessage KeyExchangeRequest();
				
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
				CoreMessage(Kind kind, State state);
				
				Kind kind_;
				State state_;
				Endpoint endpoint_;
				std::vector<NetworkId> networks_;
				
			
		};
		
	}
	
}

#endif
