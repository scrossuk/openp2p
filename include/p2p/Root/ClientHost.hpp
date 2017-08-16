#ifndef P2P_ROOTNETWORK_CLIENTHOST_HPP
#define P2P_ROOTNETWORK_CLIENTHOST_HPP

#include <set>
#include <unordered_map>
#include <vector>

#include <p2p/Root/Root_api.hpp>
#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Key.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/NodeInfo.hpp>
#include <p2p/Root/PublicIdentity.hpp>

namespace p2p {

	namespace Root {
		
		class ClientHostDelegate: public IdentityDelegate, public NodeDetectDelegate {
			public:
				virtual Socket<UDP::Endpoint, Buffer>& getUDPSocket() = 0;
				
		};
		
		class ClientHost {
			public:
				ClientHost(ClientHostDelegate& delegate);
				~ClientHost();
				
				RoutineIdGenerator& idGenerator();
				
				MultiplexHost<Root::NodePair, Root::Message>& socketHost();
				
				
				
		};
		
	}
	
}

#endif
