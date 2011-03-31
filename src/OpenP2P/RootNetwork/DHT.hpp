#ifndef OPENP2P_ROOTNETWORK_DHT_HPP
#define OPENP2P_ROOTNETWORK_DHT_HPP

#include <vector>

#include <boost/utility.hpp>

#include <OpenP2P/RPCProtocol.hpp>
#include <OpenP2P/Socket.hpp>

#include <OpenP2P/Kademlia/BucketSet.hpp>
#include <OpenP2P/Kademlia/Database.hpp>
#include <OpenP2P/Kademlia/NodeQueue.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Id.hpp>
#include <OpenP2P/RootNetwork/Node.hpp>
#include <OpenP2P/RootNetwork/Parameters.hpp>
#include <OpenP2P/RootNetwork/RPCSocket.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		class DHT: boost::noncopyable {
			public:
				DHT(Socket& socket, const IdType& id);
				
				boost::optional<Node> addEndpoint(const Endpoint& endpoint);

				std::vector<Node> findNearest(const IdType& id);
				
				boost::optional<Node> findNode(const Id& id);
				
				bool subscribe(const Id& subscriptionId);
				
				std::vector<Node> getSubscribers(const Id& subscriptionId);

				void cancel();
				
			private:
				Id id_;
				RPCSocket rpcSocket;
				Kademlia::BucketSet<Endpoint, IdSize, BucketSize> bucketSet_;
				RPCProtocol<Endpoint, Id> protocol_;
				
		};
		
	}
	
}

#endif
