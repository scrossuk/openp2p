#ifndef P2P_ROOTNETWORK_NODEINFO_HPP
#define P2P_ROOTNETWORK_NODEINFO_HPP

#include <vector>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/NodeId.hpp>

namespace p2p {

	namespace Root {
	
		struct NodeInfo {
			NodeId id;
			std::vector<Endpoint> endpointList;
			
			static NodeInfo Read(BlockingReader& reader);
			
			inline NodeInfo(NodeId pId, std::vector<Endpoint> pEndpointList)
				: id(std::move(pId)), endpointList(std::move(pEndpointList)) { }
			
			void writeTo(BlockingWriter& writer) const;
		};
		
	}
	
}

#endif
