#include "../Core.hpp"
#include "FindNode.hpp"
#include "Network.hpp"
#include "NodeGroup.hpp"

using namespace openp2p::Core;

namespace openp2p{

	namespace DHT{

		namespace{

			void StoreRes(SharedPtr<RPCManager> rpcManager, Hash hash, Buffer buffer, boost::function<void ()> success, boost::function<void ()> fail, NodeGroup group){
				Join join;
				Activation failActivate(group.Size(), fail);
				for(NodeGroup::Iterator i = group.GetIterator(); i.IsValid(); i++){
					Node node = *i;
					rpcManager->Store(node.GetEndpoint(), hash, buffer, join.Wrap(success), failActivate);
				}
			}

			void FindValueRes(SharedPtr<RPCManager> rpcManager, Hash hash, boost::function<void (Buffer)> success, boost::function<void ()> fail, NodeGroup group){
				Join join;
				Activation failActivate(group.Size(), fail);
				for(NodeGroup::Iterator i = group.GetIterator(); i.IsValid(); i++){
					Node node = *i;
					rpcManager->FindValue(node.GetEndpoint(), hash, join.Wrap(success), failActivate);
				}
			}

			void FindNodeRes(Hash hash, boost::function<void (Node)> success, boost::function<void ()> fail, NodeGroup group){
				Node nearestNode = group.Nearest();
				if(nearestNode.GetHash() == hash){
					success(nearestNode);
				}else{
					fail();
				}
			}

		}

		const unsigned int DefaultTimeout = 2.0;

		Network::Network(SocketBuilder builder, Hash hash, Database * database) : mRPCManager(new RPCManager(builder, hash, database, DefaultTimeout)){ }

		Network::~Network(){
			mRPCManager->Destroy();
		}

		void Network::AddEndpoint(Endpoint endpoint, boost::function<void (Node)> success, boost::function<void ()> fail){
			mRPCManager->Ping(endpoint, success, fail);
		}

		void Network::Store(Hash hash, Buffer buffer, boost::function<void ()> success, boost::function<void ()> fail){
			DHT::FindNearestNodes(mRPCManager, hash, boost::bind(StoreRes, mRPCManager, hash, buffer, success, fail, _1), fail);
		}

		void Network::FindNode(Hash hash, boost::function<void (Node)> success, boost::function<void ()> fail){
			DHT::FindNearestNodes(mRPCManager, hash, boost::bind(FindNodeRes, hash, success, fail, _1), fail);
		}

		void Network::FindNearestNodes(Hash hash, boost::function<void (NodeGroup)> success, boost::function<void ()> fail){
			DHT::FindNearestNodes(mRPCManager, hash, success, fail);
		}

		void Network::FindValue(Hash hash, boost::function<void (Buffer)> success, boost::function<void ()> fail){
			DHT::FindNearestNodes(mRPCManager, hash, boost::bind(FindValueRes, mRPCManager, hash, success, fail, _1), fail);
		}

	}

}


