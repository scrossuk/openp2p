#ifndef OPENP2P_RPCPROTOCOL_HPP
#define OPENP2P_RPCPROTOCOL_HPP

#include <stdint.h>
#include <queue>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Condition.hpp>
#include <OpenP2P/IdGenerator.hpp>
#include <OpenP2P/IdMap.hpp>
#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Mutex.hpp>
#include <OpenP2P/RPCSocket.hpp>

namespace OpenP2P {

	template <typename EndpointType, typename IdType>
	class RPCProtocol : boost::noncopyable{

		/* Internal structs */
		struct Request{
			IdType rpcId;
			EndpointType endpoint;
			Buffer data;
		};

		struct Listener{
			Condition * condition;
			unsigned int * opNum;
			boost::optional<Buffer> * data;
		};

		public:
			RPCProtocol(RPCSocket<EndpointType, IdType>& socket, IdGenerator<IdType>& generator)
					: socket_(socket), idGenerator_(generator),
					internalThread_(boost::bind(&RPCProtocol<EndpointType, IdType>::internalThread, this)){ }
			
			~RPCProtocol(){
				socket_.close();
				internalThread_.join();
			}
			
			boost::optional<Buffer> execute(const EndpointType& endpoint, const Buffer& data){
				Condition condition;
				boost::optional<Buffer> replyData;
				
				Lock lock(mutex_);
				IdType id = idGenerator_.generate();
				unsigned int opNum = 1;

				Listener listener;
				listener.condition = &condition;
				listener.opNum = &opNum;
				listener.data = &replyData;
				rpcs_.insert(std::make_pair(id, listener));

				const bool isRequest = true;
				socket_.send(endpoint, isRequest, id, data);
				condition.wait(lock);
				rpcs_.erase(id);
				
				return replyData;
			}

			bool receiveRequest(IdType& rpcId, EndpointType& endpoint, Buffer& data){
				Lock lock(mutex_);
				if(receiveQueue_.empty()){
					receiveCondition_.wait(lock);
					if(receiveQueue_.empty()) return false;
				}

				Request request = receiveQueue_.front();
				rpcId = request.rpcId;
				endpoint = request.endpoint;
				data = request.data;

				receiveQueue_.pop();
				return true;
			}

			void sendReply(const IdType& rpcId, const EndpointType& endpoint, const Buffer& data){
				Lock lock(mutex_);
				const bool isRequest = false;
				socket_.send(endpoint, isRequest, rpcId, data);
			}
			
			//For variable sized groups of RPCs
			void executeGroup(const std::vector< std::pair<EndpointType, Buffer> >& requestList, std::vector< boost::optional<Buffer> >& replyList){
				Condition condition;
				std::vector<IdType> idData(requestList.size());
				replyList.resize(requestList.size());
				const bool isRequest = true;
				unsigned int opNum = requestList.size();

				Lock lock(mutex_);
				
				for(std::size_t i = 0; i < requestList.size(); i++){
					idData[i] = idGenerator_.generate();
					Listener listener;
					listener.condition = &condition;
					listener.opNum = &opNum;
					listener.data = &(replyList[i]);
					rpcs_.insert(std::make_pair(idData[i], listener));
					
					socket_.send(requestList[i].first, isRequest, idData[i], requestList[i].second);
				}

				condition.wait(lock);
				
				for(std::size_t i = 0; i < requestList.size(); i++){
					rpcs_.erase(idData[i]);
				}
			}
			
			//More efficient (stack-based) form for fixed size groups of RPCs
			template <std::size_t N>
			void executeFixedSizeGroup(const boost::array<std::pair<EndpointType, Buffer>, N>& requestList, boost::array<boost::optional<Buffer>, N>& replyList){
				Condition condition;
				boost::array<IdType, N> idData;
				const bool isRequest = true;
				unsigned int opNum = N;
				
				Lock lock(mutex_);
				for(std::size_t i = 0; i < N; i++){
					idData[i] = idGenerator_.generate();
					Listener listener;
					listener.condition = &condition;
					listener.opNum = &opNum;
					listener.data = &(replyList[i]);
					rpcs_.insert(std::make_pair(idData[i], listener));
					
					socket_.send(requestList[i].first, isRequest, idData[i], requestList[i].second);
				}
				
				condition.wait(lock);
				
				for(std::size_t i = 0; i < N; i++){
					rpcs_.erase(idData[i]);
				}
			}

			void cancel(){
				Lock lock(mutex_);
				for(typename std::map<IdType, Condition *>::iterator i = rpcs_.begin(); i != rpcs_.end(); ++i){
					i->second->notify();
				}
			}

		private:
			void internalThread() {
				EndpointType endpoint;
				Buffer data;
				IdType rpcId;
				bool isRequest;

				while (socket_.receive(endpoint, isRequest, rpcId, data)) {
					Lock lock(mutex_);
					if(isRequest){
						//It's a request -> send it to the handler
						Request request;
						request.rpcId = rpcId;
						request.endpoint = endpoint;
						request.data = data;
						if(receiveQueue_.empty()) receiveCondition_.notify();
						receiveQueue_.push(request);
					}else{
						//It's a reply -> notify any listeners
						typename std::map<IdType, Listener>::iterator it = rpcs_.find(rpcId);
					
						if(it != rpcs_.end()){
							Listener listener = it->second;
							*(listener.data) = boost::make_optional(data);
							if(*(listener.opNum) == 1){
								(listener.condition)->notify();
							}else{
								(*(listener.opNum))--;
							}
						}
					}
				}
			}

			Mutex mutex_;
			RPCSocket<EndpointType, IdType>& socket_;
			std::queue<Request> receiveQueue_;
			Condition receiveCondition_;
			IdGenerator<IdType>& idGenerator_;
			std::map<IdType, Listener> rpcs_;
			//IdMap<IdType, std::pair<Condition *, boost::optional<Buffer> *> > rpcs_;
			boost::thread internalThread_;

	};

}

#endif
