#ifndef P2P_ROOTNETWORK_DHT_SERVERDELEGATE_HPP
#define P2P_ROOTNETWORK_DHT_SERVERDELEGATE_HPP

#include <vector>

#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/NodeInfo.hpp>

namespace p2p {

	namespace Root {
	
		namespace DHT {
		
			class ServerDelegate {
				public:
					virtual std::vector<NodeInfo> getNearestNodes(const NodeId& targetId) = 0;
					
					virtual void subscribe(const NodeId& targetId, const NodeInfo& nodeInfo) = 0;
					
					virtual std::vector<NodeInfo> getSubscribers(const NodeId& targetId) = 0;
					
			};
			
		}
		
	}
	
}

#endif
