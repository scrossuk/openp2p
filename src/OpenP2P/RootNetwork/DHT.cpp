#include <set>
#include <vector>

#include <OpenP2P/BinaryStream.hpp>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/BufferBuilder.hpp>
#include <OpenP2P/BufferIterator.hpp>
#include <OpenP2P/Socket.hpp>

#include <OpenP2P/RPC/Call.hpp>
#include <OpenP2P/RPC/Group.hpp>
#include <OpenP2P/RPC/Protocol.hpp>

#include <OpenP2P/RootNetwork/DHT.hpp>
#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Id.hpp>
#include <OpenP2P/RootNetwork/Node.hpp>
#include <OpenP2P/RootNetwork/Parameters.hpp>
#include <OpenP2P/RootNetwork/RPCProbeGroup.hpp>
#include <OpenP2P/RootNetwork/RPCs.hpp>
#include <OpenP2P/RootNetwork/RPCType.hpp>
#include <OpenP2P/RootNetwork/RPCWrapper.hpp>

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

		DHT::InternalThread::InternalThread(DHT& dht) : dht_(dht){ }

		void DHT::InternalThread::run(){
			Id senderId;
			RPCType type;
			std::map<Id, std::set<Node> > subscribers;
			UDP::Endpoint endpoint;
			Id rpcId;
			Buffer buffer;

			while(dht_.protocol_.receiveRequest(endpoint, rpcId, buffer)){
				BufferIterator iterator(buffer);
				BinaryStream stream(iterator);
				if(!(stream >> senderId >> type)) continue;

				Node senderNode(senderId, endpoint);

				dht_.bucketSet_.add(senderNode);

				switch(type){
					case RPC_FINDNODE:
						{
							FindNode::Request request;
							if(!(stream >> request)) continue;

							FindNode::Reply reply;
							reply.nearestGroup = dht_.bucketSet_.getNearest(request.nodeId, MaxGroupSize);
							RPCWrapper<FindNode::Reply> replyWrapper(dht_.id_, RPC_FINDNODE, reply);

							dht_.protocol_.sendReply(endpoint, rpcId, toBuffer(replyWrapper));
						}
						break;
					case RPC_PING:
						{
							RPCWrapper<Ping::Reply> replyWrapper(dht_.id_, RPC_PING, Ping::Reply());

							dht_.protocol_.sendReply(endpoint, rpcId, toBuffer(replyWrapper));
						}
						break;
					case RPC_GETSUBSCRIBERS:
						{
							GetSubscribers::Request request;
							if(!(stream >> request)) continue;

							GetSubscribers::Reply reply;
							std::set<Node>& subscribersSet = subscribers[request.subscriptionId];
							reply.subscribersGroup = std::vector<Node>(subscribersSet.begin(), subscribersSet.end());
							RPCWrapper<GetSubscribers::Reply> replyWrapper(dht_.id_, RPC_GETSUBSCRIBERS, reply);

							dht_.protocol_.sendReply(endpoint, rpcId, toBuffer(replyWrapper));
						}
						break;
					case RPC_SUBSCRIBE:
						{
							Subscribe::Request request;
							if(!(stream >> request)) continue;

							subscribers[request.subscriptionId].insert(senderNode);
						}
						break;
					default:
						//unrecognised rpc
						break;
				}
			}
		}

		void DHT::InternalThread::cancel(){
			dht_.protocol_.close();
		}

		DHT::DHT(Socket<Endpoint>& socket, const Id& id)
			: id_(id), rpcSocket_(socket), protocol_(rpcSocket_, idGenerator_), bucketSet_(id), internalThread_(*this), threadType_(internalThread_) { }

		boost::optional<Node> DHT::addEndpoint(const Endpoint& endpoint) {
			RPCWrapper<Ping::Request> request(id_, RPC_PING, Ping::Request());

			RPC::Call<Endpoint, Id> call(protocol_, endpoint, toBuffer(request));

			boost::optional<Buffer> replyBuffer = call.execute();

			if(replyBuffer) {
				RPCWrapper<Ping::Reply> replyWrapper;

				if(fromBuffer(replyWrapper, *replyBuffer)){
					Node node(replyWrapper.senderId, endpoint);
					bucketSet_.add(node);
					return boost::make_optional(node);
				}
			}

			return boost::optional<Node>();
		}

		std::vector<Node> DHT::findNearest(const Id& nodeId, std::size_t numNodes) {
			Kademlia::NodeQueue<Endpoint, IdSize> queue(nodeId);
			queue.add(bucketSet_.getNearest(nodeId, MaxGroupSize));

			while(!queue.isNearestVisited()){
				FindNode::Request request;
				request.nodeId = nodeId;
				RPCWrapper<FindNode::Request> requestWrapper(id_, RPC_FINDNODE, request);

				RPCProbeGroup probeGroup(protocol_, queue, toBuffer(requestWrapper));

				probeGroup.execute();

				for(std::size_t i = 0; i < probeGroup.size(); i++){
					if(!probeGroup.hasReply(i)) continue;

					RPCWrapper<FindNode::Reply> replyWrapper;

					if(fromBuffer(replyWrapper, probeGroup.getReply(i))){
						queue.add(replyWrapper.rpc.nearestGroup);
					}
				}
			}

			return queue.getNearest(numNodes);
		}

		boost::optional<Node> DHT::findNode(const Id& nodeId) {
			const std::size_t numNodes = 1;
			std::vector<Node> nodeArray = findNearest(nodeId, numNodes);
			if(!nodeArray.empty()){
				if(nodeArray[0].id == nodeId){
					return boost::make_optional(nodeArray[0]);
				}
			}

			return boost::optional<Node>();
		}

		bool DHT::subscribe(const Id& subscriptionId) {
			std::vector<Node> nodeArray = findNearest(subscriptionId);

			Subscribe::Request request;
			request.subscriptionId = subscriptionId;
			RPCWrapper<Subscribe::Request> requestWrapper(id_, RPC_SUBSCRIBE, request);

			RPC::Group<Endpoint, Id> rpcGroup(protocol_);

			for(std::size_t i = 0; i < nodeArray.size(); i++) {
				rpcGroup.add(nodeArray[i].endpoint, toBuffer(requestWrapper));
			}

			rpcGroup.execute();

			return true;
		}

		std::vector<Node> DHT::getSubscribers(const Id& subscriptionId, std::size_t numSubscribers) {
			std::vector<Node> nodeArray = findNearest(subscriptionId);

			GetSubscribers::Request request;
			request.subscriptionId = subscriptionId;

			RPCWrapper<GetSubscribers::Request> requestWrapper(id_, RPC_GETSUBSCRIBERS, request);

			RPC::Group<Endpoint, Id> rpcGroup(protocol_);

			for(std::size_t i = 0; i < nodeArray.size(); i++){
				rpcGroup.add(nodeArray[i].endpoint, toBuffer(requestWrapper));
			}

			rpcGroup.execute();

			std::set<Node> subscribersSet;

			for(std::size_t i = 0; i < rpcGroup.size(); i++) {
				if(!rpcGroup.hasReply(i)) continue;

				RPCWrapper<GetSubscribers::Reply> replyWrapper;

				if(fromBuffer(replyWrapper, rpcGroup.getReply(i))){
					std::vector<Node>& subscribersGroup = replyWrapper.rpc.subscribersGroup;
					subscribersSet.insert(subscribersGroup.begin(), subscribersGroup.end());
				}
			}

			std::vector<Node> subscribersArray;

			for(std::set<Node>::iterator i = subscribersSet.begin(); i != subscribersSet.end() && subscribersArray.size() < numSubscribers; ++i){
				subscribersArray.push_back(*i);
			}

			return subscribersArray;
		}

		void DHT::cancel(){
			//TODO
		}

	}

}


