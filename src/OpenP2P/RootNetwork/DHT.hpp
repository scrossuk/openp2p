#ifndef OPENP2P_ROOTNETWORK_DHT_HPP
#define OPENP2P_ROOTNETWORK_DHT_HPP

#include <vector>

#include <boost/utility.hpp>

#include <OpenP2P/Runnable.hpp>
#include <OpenP2P/Socket.hpp>
#include <OpenP2P/Thread.hpp>

#include <OpenP2P/RPC/Protocol.hpp>

#include <OpenP2P/Kademlia/BucketSet.hpp>
#include <OpenP2P/Kademlia/NodeQueue.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Id.hpp>
#include <OpenP2P/RootNetwork/IdGenerator.hpp>
#include <OpenP2P/RootNetwork/Node.hpp>
#include <OpenP2P/RootNetwork/Parameters.hpp>
#include <OpenP2P/RootNetwork/RPCSocket.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		class DHT: boost::noncopyable {
				class InternalThread: public Runnable {
					public:
						InternalThread(DHT& dht);
						
						void run();
						
						void cancel();
						
					private:
						DHT& dht_;
						
				};
				
			public:
				DHT(Socket<Endpoint>& socket, const Id& id);
				
				boost::optional<Node> addEndpoint(const Endpoint& endpoint);
				
				std::vector<Node> findNearest(const Id& id, size_t numNodes = MaxGroupSize);
				
				boost::optional<Node> findNode(const Id& id);
				
				bool subscribe(const Id& subscriptionId);
				
				std::vector<Node> getSubscribers(const Id& subscriptionId, size_t numNodes = MaxGroupSize);
				
				void cancel();
				
			private:
				Id id_;
				RPCSocket rpcSocket_;
				IdGenerator idGenerator_;
				RPC::Protocol<Endpoint, Id> protocol_;
				Kademlia::BucketSet<Endpoint, IdSize, MaxBucketSize> bucketSet_;
				InternalThread internalThread_;
				Thread threadType_;
				
		};
		
	}
	
}

#endif
