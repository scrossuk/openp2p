#ifndef P2P_ROOTNETWORK_NODEINFO_HPP
#define P2P_ROOTNETWORK_NODEINFO_HPP

#include <set>

#include <p2p/Root/Root_api.hpp>
#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/NodeId.hpp>

namespace p2p {

	namespace Root {
	
		struct NodeInfo {
			NodeId id;
			std::set<Endpoint> endpointSet;
			
			static NodeInfo Read(BlockingReader& reader);
			
			inline NodeInfo(NodeId pId, std::set<Endpoint> pEndpointSet)
				: id(std::move(pId)), endpointSet(std::move(pEndpointSet)) { }
			
			void writeTo(BlockingWriter& writer) const;
		};
		
	}
	
}

#endif
