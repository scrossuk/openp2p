#ifndef P2P_RPC_HOST_HPP
#define P2P_RPC_HOST_HPP

#include <mutex>
#include <unordered_map>

#include <p2p/Event/Source.hpp>
#include <p2p/Event/Wait.hpp>

#include <p2p/RPC/HostAPI.hpp>
#include <p2p/RPC/Operation.hpp>
#include <p2p/RPC/OperationData.hpp>

namespace p2p {

	namespace RPC {
	
		template <typename ResultType, typename TokenType>
		class Host: public HostAPI<ResultType, TokenType> {
			public:
				using DataType = OperationData<ResultType, TokenType>;
				
				Host() { }
				~Host() { }
				
				Operation<ResultType> startOperation(const TokenType& token) {
					auto operationData = std::unique_ptr<DataType>(new DataType(*this, token));
					return Operation<ResultType>(std::move(operationData));
				}
				
				void completeOperation(const TokenType& token, ResultType result) {
					std::lock_guard<std::mutex> lock(mutex_);
					const auto iterator = dataMap_.find(token);
					if (iterator == dataMap_.end()) return;
					iterator->second->operationComplete(std::move(result));
					dataMap_.erase(iterator);
				}
				
			private:
				void add(const TokenType& token, PushAPI<ResultType>* pushData) {
					std::lock_guard<std::mutex> lock(mutex_);
					dataMap_.emplace(token, pushData);
				}
				
				void remove(const TokenType& token) {
					std::lock_guard<std::mutex> lock(mutex_);
					dataMap_.erase(token);
				}
				
				std::mutex mutex_;
				std::unordered_map<TokenType, PushAPI<ResultType>*> dataMap_;
			
		};
		
	}
	
}

#endif
