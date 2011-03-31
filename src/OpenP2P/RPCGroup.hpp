#ifndef OPENP2P_RPCGROUP_HPP
#define OPENP2P_RPCGROUP_HPP

#include <stdint.h>
#include <vector>

#include <boost/optional.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/RPCProtocol.hpp>

namespace OpenP2P {

	template <typename EndpointType, typename IdType>
	class RPCGroup : boost::noncopyable{
		public:
			RPCGroup(RPCProtocol<EndpointType, IdType>& protocol) : protocol_(protocol){ }
			
			void add(const EndpointType& endpoint, const Buffer& buffer){
				requestList_.push_back(std::make_pair(endpoint, buffer));
				replyList_.push_back(boost::optional<Buffer>());
			}
			
			std::size_t size(){
				return requestList_.size();
			}
			
			void execute(){
				protocol_.executeGroup(requestList_, replyList_);
			}

			bool hasReply(std::size_t i){
				return replyList_[i];
			}

			Buffer getReply(std::size_t i){
				return *(replyList_[i]);
			}

			void cancel(){
				protocol_.cancel();
			}
				
		private:
			RPCProtocol<EndpointType, IdType>& protocol_;
			std::vector< std::pair<EndpointType, Buffer> > requestList_;
			std::vector< boost::optional<Buffer> > replyList_;

	};

}

#endif
