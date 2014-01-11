#ifndef OPENP2P_RPC_PROTOCOL_HPP
#define OPENP2P_RPC_PROTOCOL_HPP

#include <stdint.h>
#include <queue>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Condition.hpp>
#include <OpenP2P/IdGenerator.hpp>
#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Mutex.hpp>
#include <OpenP2P/Signal.hpp>

#include <OpenP2P/RPC/Socket.hpp>

namespace OpenP2P {

	namespace RPC {
	
		template <typename EndpointType, typename IdType>
		class Protocol : boost::noncopyable {
				struct RequestData {
					EndpointType endpoint;
					IdType rpcId;
					Buffer data;
				};
				
			public:
				//Allows listening for multiple replies
				struct OpNotifier {
					Signal* signal;
					size_t* activeOpCount;
				};
				
				//Listening for a reply
				struct ReplyNotifier {
					OpNotifier* opNotifier;
					boost::optional<Buffer>* data;
				};
				
				Protocol(Socket<EndpointType, IdType>& socket, IdGenerator<IdType>& generator)
					: socket_(socket), idGenerator_(generator), closed_(false),
					  internalThread_(boost::bind(&Protocol<EndpointType, IdType>::internalThread, this)) { }
					  
				~Protocol() {
					socket_.close();
					internalThread_.join();
				}
				
				IdType generateId() {
					Lock lock(mutex_);
					return idGenerator_.generate();
				}
				
				void addReplyNotifier(const IdType& id, ReplyNotifier* replyNotifier) {
					Lock lock(mutex_);
					replyNotifiers_.insert(std::make_pair(id, replyNotifier));
				}
				
				void removeReplyNotifier(const IdType& id) {
					Lock lock(mutex_);
					replyNotifiers_.erase(id);
				}
				
				void sendRequest(const EndpointType& endpoint, const IdType& id, const Buffer& data) {
					const bool isRequest = true;
					Lock lock(mutex_);
					socket_.send(endpoint, isRequest, id, data);
				}
				
				void sendReply(const EndpointType& endpoint, const IdType& id, const Buffer& data) {
					const bool isRequest = false;
					Lock lock(mutex_);
					socket_.send(endpoint, isRequest, id, data);
				}
				
				bool receiveRequest(EndpointType& endpoint, IdType& id, Buffer& data) {
					Lock lock(mutex_);
					
					if (closed_) {
						return false;
					}
					
					if (requestQueue_.empty()) {
						requestCondition_.wait(lock);
						
						if (requestQueue_.empty()) {
							return false;
						}
					}
					
					RequestData request = requestQueue_.front();
					endpoint = request.endpoint;
					id = request.rpcId;
					data = request.data;
					
					requestQueue_.pop();
					return true;
				}
				
				void close() {
					socket_.close();
					
					Lock lock(mutex_);
					
					closed_ = true;
					
					for (typename std::map<IdType, ReplyNotifier*>::iterator i = replyNotifiers_.begin(); i != replyNotifiers_.end(); ++i) {
						i->second->opNotifier->signal->activate();
					}
					
					requestCondition_.notifyOne();
				}
				
				bool isClosed() {
					Lock lock(mutex_);
					return closed_;
				}
				
			private:
				void internalThread() {
					EndpointType endpoint;
					Buffer data;
					IdType rpcId;
					bool isRequest;
					
					while (socket_.receive(endpoint, isRequest, rpcId, data)) {
						Lock lock(mutex_);
						
						if (isRequest) {
							RequestData request;
							request.rpcId = rpcId;
							request.endpoint = endpoint;
							request.data = data;
							
							if (requestQueue_.empty()) {
								requestCondition_.notifyOne();
							}
							
							requestQueue_.push(request);
						} else {
							typename std::map<IdType, ReplyNotifier*>::iterator it = replyNotifiers_.find(rpcId);
							
							if (it != replyNotifiers_.end()) {
								ReplyNotifier* notifier = it->second;
								*(notifier->data) = boost::make_optional(data);
								
								if (*(notifier->opNotifier->activeOpCount) == 1) {
									notifier->opNotifier->signal->activate();
								} else {
									(*(notifier->opNotifier->activeOpCount))--;
								}
							}
						}
					}
					
					close();
				}
				
				Mutex mutex_;
				Socket<EndpointType, IdType>& socket_;
				std::queue<RequestData> requestQueue_;
				Condition requestCondition_;
				IdGenerator<IdType>& idGenerator_;
				bool closed_;
				std::map<IdType, ReplyNotifier*> replyNotifiers_;
				boost::thread internalThread_;
				
		};
		
	}
	
}

#endif
