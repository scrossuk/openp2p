#ifndef P2P_ROOTNETWORK_NODEPAIR_HPP
#define P2P_ROOTNETWORK_NODEPAIR_HPP

#include <p2p/Root/Root_api.hpp>
#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/NodeId.hpp>

namespace p2p {

	namespace Root {
	
		struct NodePair {
			NodeId id;
			Endpoint endpoint;
			
			inline NodePair() { }
			
			inline NodePair(const NodeId& pId, const Endpoint& pEndpoint)
				: id(pId), endpoint(pEndpoint) { }
		};
		
	}
	
}

#endif
