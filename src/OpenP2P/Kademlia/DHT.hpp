#ifndef OPENP2P_KADEMLIA_DHT_HPP
#define OPENP2P_KADEMLIA_DHT_HPP

#include <cstddef>
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <boost/ref.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Block.hpp>
#include <OpenP2P/RPCProtocol.hpp>
#include <OpenP2P/WaitObject.hpp>
#include <OpenP2P/Kademlia/BucketSet.hpp>
#include <OpenP2P/Kademlia/Database.hpp>
#include <OpenP2P/Kademlia/FindNode.hpp>
#include <OpenP2P/Kademlia/FindValue.hpp>
#include <OpenP2P/Kademlia/Id.hpp>
#include <OpenP2P/Kademlia/Node.hpp>
#include <OpenP2P/Kademlia/NodeGroup.hpp>
#include <OpenP2P/Kademlia/NodeQueue.hpp>
#include <OpenP2P/Kademlia/Ping.hpp>
#include <OpenP2P/Kademlia/Store.hpp>

namespace OpenP2P{

	namespace Kademlia{

		const std::size_t NumProbes = 3;

		template <typename EndpointType, std::size_t IdSize>
		class DHT: boost::noncopyable{
			typedef Id<IdSize> IdType;
			typedef Node<EndpointType, IdSize> NodeType;
			typedef RPCProtocol<EndpointType, IdType> ProtocolType;
			typedef NodeGroup<EndpointType, IdSize> GroupType;
			typedef typename GroupType::Iterator GroupIterator;

			typedef typename FindNode::Request<IdSize> FindNodeRequest;
			typedef typename FindNode::Reply<EndpointType, IdSize> FindNodeReply;

			typedef typename FindValue::Request<IdSize> FindValueRequest;
			typedef FindValue::Reply FindValueReply;

			typedef Ping::Request PingRequest;
			typedef typename Ping::Reply<IdSize> PingReply;

			typedef typename Store::Request<IdSize> StoreRequest;
			typedef Store::Reply StoreReply;

			public:
				DHT(ProtocolType& protocol, const IdType& id, Database<IdSize>& database)
					: protocol_(protocol), id_(id), database_(database), bucketSet_(id),
					findNode_(protocol, "findnode", boost::bind(&DHT::onFindNode, this, _1, _2)),
					findValue_(protocol, "findvalue", boost::bind(&DHT::onFindValue, this, _1, _2)),
					ping_(protocol, "ping", boost::bind(&DHT::onPing, this, _1, _2)),
					store_(protocol, "store", boost::bind(&DHT::onStore, this, _1, _2)) { }

				boost::optional<NodeType> addEndpoint(const EndpointType& endpoint, WaitObject& waitObject = defaultBlock){
					boost::optional<PingReply> reply = ping_.send(endpoint, PingRequest());

					if(reply){
						NodeType node(reply->id, endpoint);
						bucketSet_.add(node);
						return boost::make_optional(node);
					}else{
						return boost::optional<NodeType>();
					}
				}

				bool store(const IdType& id, const Buffer& buffer, WaitObject& waitObject = defaultBlock){
					GroupType group = findNearest(id);

					for(GroupIterator i = group.iterator(); i.isValid(); i++){
						store_.send((*i).endpoint, StoreRequest(id, buffer));
					}

					return true;
				}

				boost::optional<NodeType> findNode(const IdType& id, WaitObject& waitObject = defaultBlock){
					GroupType group = findNearest(id);
					boost::optional<NodeType> node = group.nearest();
					if(node){
						if(node->id == id){
							return node;
						}
					}

					return boost::optional<NodeType>();
				}

				GroupType bucketNearest(const IdType& id){
					return bucketSet_.nearest(id, 20);
				}

				GroupType findNearest(const IdType& id){
					GroupType group = bucketSet_.nearest(id, 20);
					NodeQueue<EndpointType, IdSize> queue(id);

					if(!group.empty()){
						queue.push(group);

						boost::barrier barrier(NumProbes);

						boost::thread_group threads;
						for(std::size_t i = 0; i < NumProbes; i++){
							threads.create_thread(boost::bind(&DHT::probe, this, id, boost::ref(queue), boost::ref(barrier)));
						}
						threads.join_all();
					}

					return queue.group();
				}

				boost::optional<Buffer> findValue(const Id<IdSize>& id, WaitObject& waitObject = defaultBlock){
					GroupType group = findNearest(id);

					for(GroupIterator i = group.iterator(); i.isValid(); ++i){
						NodeType node = *i;
						boost::optional<FindValueReply> reply = findValue_.send(node.endpoint, FindValueRequest(id));

						if(reply){
							if(reply->data){
								return reply->data;
							}
						}
					}

					return boost::optional<Buffer>();
				}

			private:
				void probe(const IdType& id, NodeQueue<EndpointType, IdSize>& queue, boost::barrier& barrier){
					while(true){
						IdType previousDistance = queue.distance();

						barrier.wait();

						boost::optional<NodeType> node = queue.pop();

						if(node){
							boost::optional<FindNodeReply> reply = findNode_.send(node->endpoint, FindNodeRequest(id));
							if(reply){
								queue.push(reply->group);
							}
						}

						barrier.wait();

						IdType currentDistance = queue.distance();

						if(previousDistance == currentDistance){
							//No progression on this iteration, so end
							return;
						}
					}
				}

				FindNodeReply onFindNode(const EndpointType&, const FindNodeRequest& request) {
					return FindNodeReply(bucketSet_.nearest(request.nodeId, 20));
				}

				FindValueReply onFindValue(const EndpointType&, const FindValueRequest& request) {
					return FindValueReply(database_.retrieve(request.dataId));
				}

				PingReply onPing(const EndpointType& endpoint, const PingRequest& request) {
					return PingReply(id_);
				}

				StoreReply onStore(const EndpointType&, const StoreRequest& request) {
					return StoreReply(database_.store(request.dataId, request.data));
				}

				ProtocolType& protocol_;
				IdType id_;
				Database<IdSize>& database_;
				BucketSet<EndpointType, IdSize> bucketSet_;
				typename ProtocolType::template Handler< FindNodeRequest, FindNodeReply > findNode_;
				typename ProtocolType::template Handler< FindValueRequest, FindValueReply > findValue_;
				typename ProtocolType::template Handler< PingRequest, PingReply > ping_;
				typename ProtocolType::template Handler< StoreRequest, StoreReply > store_;

		};

	}

}

#endif
