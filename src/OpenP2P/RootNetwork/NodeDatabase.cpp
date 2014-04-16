#include <unordered_map>

#include <OpenP2P/RootNetwork/Key.hpp>
#include <OpenP2P/RootNetwork/NodeDatabase.hpp>
#include <OpenP2P/RootNetwork/NodeId.hpp>
#include <OpenP2P/RootNetwork/PublicIdentity.hpp>

namespace OpenP2P {

	namespace RootNetwork {
		
		NodeDatabase::NodeDatabase() { }
		
		bool NodeDatabase::isKnownId(const NodeId& id) const {
			return map_.find(id) != map_.end();
		}
		
		void NodeDatabase::addNode(const NodeId& id, NodeInfo pNodeInfo) {
			map_.emplace(id, std::move(pNodeInfo));
		}
		
		NodeInfo& NodeDatabase::nodeInfo(const NodeId& id) {
			return map_.at(id);
		}
		
		const NodeInfo& NodeDatabase::nodeInfo(const NodeId& id) const {
			return map_.at(id);
		}
		
	}
	
}

