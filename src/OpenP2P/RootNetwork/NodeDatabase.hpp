#ifndef OPENP2P_ROOTNETWORK_NODEDATABASE_HPP
#define OPENP2P_ROOTNETWORK_NODEDATABASE_HPP

#include <set>
#include <unordered_map>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Key.hpp>
#include <OpenP2P/RootNetwork/NodeId.hpp>
#include <OpenP2P/RootNetwork/PublicIdentity.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
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
