#ifndef OPENP2P_ROOTNETWORK_SERVICE_HPP
#define OPENP2P_ROOTNETWORK_SERVICE_HPP

namespace OpenP2P {

	namespace RootNetwork {
	
		class Service {
			public:
				Service(PrivateIdentity& identity);
				
				bool setUDPSocket(UDP::Socket& socket);
				
				boost::optional<Node> identifyEndpoint(const Endpoint& endpoint, Timeout timeout = Timeout::Infinite());
				
				boost::optional<Node> findNode(const Id& id, Timeout timeout = Timeout::Infinite());
				
				boost::optional< std::vector<Node> > findNearestNodes(const Id& id, Timeout timeout = Timeout::Infinite());
				
				bool subscribe(const SubNetworkId& subNetworkId, const Buffer& subNetworkInfo, Timeout timeout = Timeout::Infinite());
				
				boost::optional< std::vector<Node> > getSubscribers(const SubNetworkId& subNetworkId, Timeout timeout = Timeout::Infinite());
				
				boost::optional<Buffer> getSubNetworkInfo(const Id& id, const SubNetworkId& subNetworkId, Timeout timeout = Timeout::Infinite());
				
			private:
				PrivateIdentity& identity_;
				
		};
		
	}
	
}

#endif
