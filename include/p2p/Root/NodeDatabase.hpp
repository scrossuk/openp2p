#ifndef P2P_ROOTNETWORK_NODEDATABASE_HPP
#define P2P_ROOTNETWORK_NODEDATABASE_HPP

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
	
		struct NodeEntry {
			PublicIdentity identity;
			std::set<Endpoint> endpointSet;
			
			inline NodeEntry(PublicIdentity pIdentity)
				: identity(std::move(pIdentity)) { }
			
			inline NodeId id() const {
				return identity.id();
			}
			
			inline NodeInfo toNodeInfo() const {
				return NodeInfo(id(), endpointSet);
			}
		};
		
		class ROOT_OPENP2P_API NodeDatabase {
			public:
				NodeDatabase();
				
				bool isKnownId(const NodeId& id) const;
				
				void addNode(const NodeId& id, NodeEntry nodeInfo);
				
				NodeEntry& nodeEntry(const NodeId& id);
				
				const NodeEntry& nodeEntry(const NodeId& id) const;
				
				inline const std::unordered_map<NodeId, NodeEntry>& map() const {
					return map_;
				}
				
			private:
				// Non-copyable.
				NodeDatabase(const NodeDatabase&) = delete;
				NodeDatabase& operator=(const NodeDatabase&) = delete;
				
				std::unordered_map<NodeId, NodeEntry> map_;
				
		};
		
	}
	
}

#endif
