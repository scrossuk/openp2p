#ifndef OPENP2P_RPC_GROUP_HPP
#define OPENP2P_RPC_GROUP_HPP

#include <stdint.h>
#include <vector>

#include <boost/optional.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Signal.hpp>
#include <OpenP2P/RPC/Protocol.hpp>

namespace OpenP2P {

	namespace RPC {
	
		template <typename EndpointType, typename IdType>
		class Group : boost::noncopyable {
				typedef typename Protocol<EndpointType, IdType>::OpNotifier OpNotifier;
				typedef typename Protocol<EndpointType, IdType>::ReplyNotifier ReplyNotifier;
				
				class IdArray : boost::noncopyable {
					public:
						IdArray(Protocol<EndpointType, IdType>& protocol, Signal& signal) : protocol_(protocol), activeOpCount_(0) {
							opNotifier_.signal = &signal;
							opNotifier_.activeOpCount = &activeOpCount_;
						}
						
						~IdArray() {
							for (size_t i = 0; i < idArray_.size(); i++) {
								protocol_.removeReplyNotifier(idArray_[i]);
							}
							
							for (size_t i = 0; i < replyNotifierArray_.size(); i++) {
								delete replyNotifierArray_[i];
							}
						}
						
						IdType listen(boost::optional<Buffer>& data) {
							//Initialise the notifier
							ReplyNotifier* replyNotifier = new ReplyNotifier;
							replyNotifier->opNotifier = &opNotifier_;
							replyNotifier->data = &data;
							replyNotifierArray_.push_back(replyNotifier);
							
							//Generate an id
							IdType id = protocol_.generateId();
							
							//Start listening
							protocol_.addReplyNotifier(id, replyNotifierArray_.back());
							activeOpCount_++;
							idArray_.push_back(id);
							
							return id;
						}
						
					private:
						Protocol<EndpointType, IdType>& protocol_;
						size_t activeOpCount_;
						std::vector<IdType> idArray_;
						OpNotifier opNotifier_;
						std::vector<ReplyNotifier*> replyNotifierArray_;
						
				};
				
			public:
				Group(Protocol<EndpointType, IdType>& protocol) : protocol_(protocol) { }
				
				void add(const EndpointType& endpoint, const Buffer& buffer) {
					requestArray_.push_back(std::make_pair(endpoint, &buffer));
					replyArray_.push_back(boost::optional<Buffer>());
				}
				
				size_t size() {
					return requestArray_.size();
				}
				
				void execute() {
					IdArray idArray(protocol_, signal_);
					
					for (size_t i = 0; i < requestArray_.size(); i++) {
						IdType id = idArray.listen(replyArray_[i]);
						
						protocol_.sendRequest(requestArray_[i].first, id, *(requestArray_[i].second));
					}
					
					signal_.wait();
				}
				
				bool hasReply(size_t i) {
					return replyArray_[i];
				}
				
				const Buffer& getReply(size_t i) {
					return *(replyArray_[i]);
				}
				
				void cancel() {
					signal_.activate();
				}
				
			private:
				Protocol<EndpointType, IdType>& protocol_;
				Signal signal_;
				std::vector< std::pair<EndpointType, const Buffer*> > requestArray_;
				std::vector< boost::optional<Buffer> > replyArray_;
				
		};
		
	}
	
}

#endif
