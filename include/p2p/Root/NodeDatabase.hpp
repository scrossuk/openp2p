#ifndef P2P_ROOTNETWORK_NODEDATABASE_HPP
#define P2P_ROOTNETWORK_NODEDATABASE_HPP

#include <set>
#include <unordered_map>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Key.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/PublicIdentity.hpp>

namespace p2p {

	namespace Root {
	
		struct NodeInfo {
			PublicIdentity identity;
			std::set<Endpoint> endpointSet;
			
			inline NodeInfo(PublicIdentity pIdentity)
				: identity(std::move(pIdentity)) { }
		};
		
		class NodeDatabase {
			public:
				NodeDatabase();
				
				bool isKnownId(const NodeId& id) const;
				
				void addNode(const NodeId& id, NodeInfo nodeInfo);
				
				NodeInfo& nodeInfo(const NodeId& id);
				
				const NodeInfo& nodeInfo(const NodeId& id) const;
				
			private:
				// Non-copyable.
				NodeDatabase(const NodeDatabase&) = delete;
				NodeDatabase& operator=(const NodeDatabase&) = delete;
				
				std::unordered_map<NodeId, NodeInfo> map_;
				
		};
		
	}
	
}

#endif
