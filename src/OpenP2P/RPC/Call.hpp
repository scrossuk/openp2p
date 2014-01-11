#ifndef OPENP2P_RPC_CALL_HPP
#define OPENP2P_RPC_CALL_HPP

#include <stdint.h>
#include <vector>

#include <boost/optional.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/RPC/Protocol.hpp>

namespace OpenP2P {

	namespace RPC {
	
		template <typename EndpointType, typename IdType>
		class Call : boost::noncopyable {
		
				//Internal management of Ids
				class IdListen {
					public:
						IdListen(Protocol<EndpointType, IdType>& protocol, Signal& signal, boost::optional<Buffer>& data) : protocol_(protocol), id_(protocol_.generateId()), activeOpCount_(1) {
							opNotifier_.signal = &signal;
							opNotifier_.activeOpCount = &activeOpCount_;
							
							replyNotifier_.opNotifier = &opNotifier_;
							replyNotifier_.data = &data;
							
							protocol_.addReplyNotifier(id_, &replyNotifier_);
						}
						
						~IdListen() {
							//Stop listening on the Id
							protocol_.removeReplyNotifier(id_);
						}
						
						IdType getId() const {
							return id_;
						}
						
					private:
						Protocol<EndpointType, IdType>& protocol_;
						IdType id_;
						size_t activeOpCount_;
						typename Protocol<EndpointType, IdType>::OpNotifier opNotifier_;
						typename Protocol<EndpointType, IdType>::ReplyNotifier replyNotifier_;
						
				};
				
			public:
				Call(Protocol<EndpointType, IdType>& protocol, const EndpointType& endpoint, const Buffer& buffer)
					: protocol_(protocol), endpoint_(endpoint), buffer_(buffer) { }
					
				boost::optional<Buffer> execute() {
					if (protocol_.isClosed()) {
						return boost::optional<Buffer>();
					}
					
					boost::optional<Buffer> replyData;
					IdListen idListen(protocol_, signal_, replyData);
					
					protocol_.sendRequest(endpoint_, idListen.getId(), buffer_);
					
					signal_.wait();
					
					return replyData;
				}
				
				void cancel() {
					signal_.activate();
				}
				
			private:
				Protocol<EndpointType, IdType>& protocol_;
				const EndpointType& endpoint_;
				const Buffer& buffer_;
				Signal signal_;
				
		};
		
	}
	
}

#endif
