#include <vector>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/RPCFixedGroup.hpp>
#include <OpenP2P/RPCProtocol.hpp>
#include <OpenP2P/Socket.hpp>

#include <OpenP2P/RootNetwork/DHT.hpp>
#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Id.hpp>
#include <OpenP2P/RootNetwork/Node.hpp>
#include <OpenP2P/RootNetwork/Parameters.hpp>

namespace OpenP2P {

	namespace RootNetwork {

		namespace{

			template <typename T>
			Buffer toBuffer(const T& t){
				BufferBuilder builder;
				BinaryStream stream(builder);
				stream << t;
				return builder.getBuffer();
			}

			template <typename T>
			bool fromBuffer(T& t, const Buffer& buffer){
				BufferIterator iterator(buffer);
				BinaryStream stream(iterator);
				return stream >> t;
			}

		}

		DHT::DHT(Socket& socket, const Id& id)
			: id_(id), rpcSocket_(socket), protocol_(idSocket_), bucketSet_(id) { }
			
		boost::optional<Node> DHT::addEndpoint(const Endpoint& endpoint) {
			RPCWrapper<PingRequest> request(id_, PingRequest());

			boost::optional<Buffer> replyBuffer = protocol_.execute(endpoint, toBuffer<BinaryStream>(request));
			
			if(replyBuffer) {
				RPCWrapper<PingReply> reply;
				
				if(fromBuffer(*replyBuffer, reply)){
					Node node(reply.id, endpoint);
					bucketSet_.add(node);
					return boost::make_optional(node);
				}
			}

			return boost::optional<Node>();
		}

		std::vector<Node> DHT::findNearest(const Id& id) {
			Kademlia::NodeQueue<Endpoint, IdSize> queue(nodeId);
			queue.add(bucketSet_.getNearest(nodeId));

			while(!queue.isNearestVisited()){
				RPCWrapper<FindNodeRequest> request(id_, FindNodeRequest(nodeId));
				RPCProbeGroup group(queue, toBuffer(request));

				group.execute();

				for(std::size_t i = 0; i < group.size(); i++){
					if(!group.hasReply(i)) continue;

					RPCWrapper<FindNodeReply> reply;

					if(fromBuffer(reply, group.getReply(i))){
						queue.add(reply.rpc.nearestGroup);
					}
				}
			}
			
			return queue.getNearestGroup();
		}
		
		boost::optional<Node> DHT::findNode(const Id& nodeId) {
			std::vector<Node> nearestGroup = findNearest(nodeId);
			if(!nearestGroup.empty()){
				if(nearestGroup[0].id == nodeId){
					return boost::make_optional(nearestGroup[0]);
				}
			}

			return boost::optional<Node>();
		}
		
		bool DHT::subscribe(const Id& subscriptionId) {
			std::vector<Node> nearestGroup = bucketSet_.getNearest(subscriptionId);
			
			RPCGroup rpcGroup(protocol_, nearestGroup.size());
			
			for(std::vector<Node>::iterator i = nearestGroup.begin(); i != nearestGroup.end(); ++i) {
				RPCWrapper<SubscribeRequest> request(id_, SubscribeRequest());
				
				rpcGroup.add((*i).endpoint, toBuffer(request));
			}
			
			rpcGroup.execute();
			
			return true;
		}
		
		std::set<Node> DHT::getSubscribers(const Id& subscriptionId) {
			std::vector<Node> nearestGroup = findNearest(subscriptionId);
			
			RPCGroup rpcGroup(protocol_, nearestGroup.size());
			
			for(std::vector<Node>::iterator i = nearestGroup.begin(); i != nearestGroup.end(); ++i) {
				RPCWrapper<GetSubscribersRequest> request(id_, GetSubscribersRequest(subscriptionId));
				
				rpcGroup.add((*i).endpoint, toBuffer(request));
			}
			
			rpcGroup.execute();

			std::set<Node> subscribers;
			
			for(std::size_t i = 0; i < rpcGroup.size(); i++) {
				if(!group.hasReply(i)) continue;

				RPCWrapper<GetSubscribersReply> reply;

				if(fromBuffer(reply, group.getReply(i))){
					std::vector<Node>& subscribersGroup = reply.rpc.subscribersGroup;
					subscribers.insert(subscribersGroup.begin(), subscribersGroup.end());
				}
			}

			return subscribers;
		}

		void DHT::cancel(){
			protocol_.cancel();
		}
		
	}
	
}


