#ifndef OPENP2P_ROOTNETWORK_RPCPROBEGROUP_HPP
#define OPENP2P_ROOTNETWORK_RPCPROBEGROUP_HPP

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/RPCGroup.hpp>

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
				inline RPCProbeGroup(RPCProtocol<Endpoint, Id>& protocol, QueueType& queue, const Buffer& request) : rpcGroup_(protocol), queue_(queue), request_(request){ }

				inline void execute(){
					for(std::size_t i = 0; i < NumProbes; i++){
						boost::optional<Node> node = queue_.getNearestUnvisited();
						if(node){
							rpcGroup_.add((*node).endpoint, request_);
						}else{
							break;
						}
					}

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
				RPCGroup<Endpoint, Id> rpcGroup_;
				QueueType queue_;
				Buffer request_;

		};

	}

}

#endif
