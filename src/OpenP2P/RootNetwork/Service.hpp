#ifndef OPENP2P_ROOTNETWORK_SERVICE_HPP
#define OPENP2P_ROOTNETWORK_SERVICE_HPP

#include <OpenP2P/RootNetwork/PrivateIdentity.hpp>
#include <OpenP2P/RootNetwork/PublicIdentity.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		class Service {
			public:
				Service(PrivateIdentity& identity);
				
				boost::optional<Node> identifyEndpoint(const Endpoint& endpoint, Timeout timeout = Timeout::Infinite());
				
				boost::optional<Endpoint> getNodeEndpoint(const NodeId& id, Timeout timeout = Timeout::Infinite());
				
				boost::optional<Node> findNode(const NodeId& id, Timeout timeout = Timeout::Infinite());
				
				boost::optional< std::vector<Node> > findNearestNodes(const NodeId& id, Timeout timeout = Timeout::Infinite());
				
				bool subscribe(const SubNetworkId& subNetworkId, const Buffer& subNetworkInfo, Timeout timeout = Timeout::Infinite());
				
				boost::optional< std::vector<Node> > getSubscribers(const SubNetworkId& subNetworkId, Timeout timeout = Timeout::Infinite());
				
				boost::optional<Buffer> getSubNetworkInfo(const NodeId& id, const SubNetworkId& subNetworkId, Timeout timeout = Timeout::Infinite());
				
			private:
				PrivateIdentity& identity_;
				
		};
		
	}
	
}

#endif
