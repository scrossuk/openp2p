#include "../Core.hpp"
#include "DHT.hpp"
#include "NodeGroup.hpp"
#include "Hash.hpp"
#include "Database.hpp"

using namespace openp2p::Core;

namespace openp2p{

	namespace Kademlia{
	
		namespace{
		
			const unsigned int NumProbes = 3;
			
		}

		DHT::DHT(Socket& builder, Hash hash, Database& database) : mRPCManager(socket, hash, database){ }

		Node DHT::AddEndpoint(Endpoint endpoint){
			return mRPCManager.Ping(endpoint);
		}

		void DHT::Store(Hash hash, Buffer buffer){
			NodeGroup group = FindNearestNodes(hash);
			
			for(NodeGroup::Iterator i = group.GetIterator(); i.IsValid(); i++){
				Node node = *i;
				rpcManager.Store(node.GetEndpoint(), hash, buffer);
			}
		}

		Maybe<Node> DHT::FindNode(Hash hash){
			NodeGroup group = FindNearestNodes(hash);
			Node nearestNode = group.Nearest();
			if(nearestNode.GetHash() == hash){
				return Just(nearestNode);
			}else{
				return Nothing();
			}
		}

		NodeGroup DHT::FindNearestNodes(Hash hash){
			NodeQueue queue(hash);
			NodeGroup group = mRPCManager.Nearest();
			
			if(!group.IsEmpty()){
				queue.Push(group);
				
				boost::thread_group threads;
				for(unsigned int i = 0; i < NumProbes; i++){
					threads.create_thread(boost::bind(&DHT::Probe, this, hash, boost::ref(queue)));
				}
				threads.join_all();
			}
			
			return queue.Group();
		}

		Maybe<Buffer> DHT::FindValue(Hash hash){
			NodeGroup group = FindNearestNodes(hash);
			
			for(NodeGroup::Iterator i = group.GetIterator(); i.IsValid(); i++){
				Node node = *i;
				Maybe<Buffer> r = rpcManager.FindValue(node.GetEndpoint(), hash);
				if(r){
					return r;
				}
			}
			
			return Nothing();
		}
		
		void DHT::Probe(Hash hash, NodeQueue& queue){
			while(true){
				Maybe<Node> node = queue.Pop();
				if(!node){
					return;
				}
				
				NodeGroup group = mRPCManager.FindNode(node.GetEndpoint(), hash);
				if(queue.Push(group)){
					return;
				}
			}
		}

	}

}


