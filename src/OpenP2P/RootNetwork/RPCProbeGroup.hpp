#ifndef OPENP2P_ROOTNETWORK_RPCPROBEGROUP_HPP
#define OPENP2P_ROOTNETWORK_RPCPROBEGROUP_HPP

#include <vector>

#include <OpenP2P/Buffer.hpp>

#include <OpenP2P/RPC/Group.hpp>

#include <OpenP2P/Kademlia/NodeQueue.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Id.hpp>
#include <OpenP2P/RootNetwork/Node.hpp>
#include <OpenP2P/RootNetwork/Parameters.hpp>

namespace OpenP2P{

	namespace RootNetwork{

		class RPCProbeGroup{
			typedef Kademlia::NodeQueue<Endpoint, IdSize> QueueType;

			public:
				inline RPCProbeGroup(RPC::Protocol<Endpoint, Id>& protocol, QueueType& queue, const Buffer& request) : rpcGroup_(protocol){
					std::vector<Node> nearestUnvisited = queue.getNearestUnvisited(NumProbes);
					for(std::vector<Node>::iterator i = nearestUnvisited.begin(); i != nearestUnvisited.end(); ++i){
						rpcGroup_.add(i->endpoint, request);
					}
				}

				inline void execute(){
					rpcGroup_.execute();
				}

				inline std::size_t size() const{
					return NumProbes;
				}

				inline bool hasReply(std::size_t i){
					return rpcGroup_.hasReply(i);
				}

				inline Buffer getReply(std::size_t i){
					return rpcGroup_.getReply(i);
				}

			private:
				RPC::Group<Endpoint, Id> rpcGroup_;

		};

	}

}

#endif
