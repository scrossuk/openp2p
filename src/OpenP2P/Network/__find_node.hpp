#ifndef OPENP2P_DHT_FINDNODE_HPP
#define OPENP2P_DHT_FINDNODE_HPP

#include <set>
#include <boost/function.hpp>
#include "../Core.hpp"
#include "RPCManager.hpp"
#include "Hash.hpp"
#include "Node.hpp"
#include "NodeGroup.hpp"

using namespace openp2p::Core;

namespace openp2p{

	namespace Kademlia{

		class FindNodeOp{
			public:
				FindNodeOp(SharedPtr<RPCManager> rpcManager, Hash hash, boost::function<void (NodeGroup)> success, boost::function<void ()> fail)
					: mRPCManager(rpcManager), mHash(hash), mSuccess(success), mFail(fail), mNodeGroup(hash){

					mRPCManager->Nearest(hash, boost::bind(&FindNodeOp::OnNearest, this, _1), boost::bind(&FindNodeOp::OnFail, this));
				}

				void OnFail(){
					mFail();
					delete this;
				}

				void OnNearest(NodeGroup nodeGroup){
					mNodeGroup.Add(nodeGroup);

					mActiveProbes = DispatchProbes(3);
					if(mActiveProbes == 0){
						OnFail();
						return;
					}
				}

				void OnProbeSuccess(NodeGroup nodeGroup){
					if(nodeGroup < mNodeGroup){
						mNodeGroup.Add(nodeGroup);
						if(DispatchProbes(1) == 0){
							DropProbe();
						}
					}else{
						DropProbe();
					}
				}

				void OnProbeFail(){
					DropProbe();
				}

				unsigned int DispatchProbes(unsigned int n){
					NodeGroup::Iterator p = mNodeGroup.GetIterator();
					unsigned int i = 0;

					while(p.IsValid() && i < n){
						Node node = *p;

						if(mVisited.find(node) == mVisited.end()){
							mVisited.insert(node);
							mRPCManager->FindNode(node.GetEndpoint(), mHash, mStrand.Wrap(boost::bind(&FindNodeOp::OnProbeSuccess, this, _1)), mStrand.Wrap(boost::bind(&FindNodeOp::OnProbeFail, this)));
							i++;
						}

						p++;
					}

					return i;
				}

				void DropProbe(){
					mActiveProbes--;
					if(mActiveProbes == 0){
						mSuccess(mNodeGroup);
						delete this;
					}
				}

			private:
				Strand mStrand;
				SharedPtr<RPCManager> mRPCManager;
				Hash mHash;
				boost::function<void (NodeGroup)> mSuccess;
				boost::function<void ()> mFail;
				NodeGroup mNodeGroup;
				std::set<Node> mVisited;
				unsigned int mActiveProbes;

		};

		inline void FindNearestNodes(SharedPtr<RPCManager> rpcManager, Hash hash, boost::function<void (NodeGroup)> success, boost::function<void ()> fail){
			new FindNodeOp(rpcManager, hash, success, fail);
		}

	}

}

#endif
