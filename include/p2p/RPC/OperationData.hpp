#ifndef P2P_RPC_OPERATIONDATA_HPP
#define P2P_RPC_OPERATIONDATA_HPP

#include <mutex>
#include <stdexcept>

#include <boost/optional.hpp>

#include <p2p/Event/Generator.hpp>
#include <p2p/Event/Source.hpp>

#include <p2p/RPC/HostAPI.hpp>
#include <p2p/RPC/PushAPI.hpp>
#include <p2p/RPC/RetrieveAPI.hpp>

namespace p2p {

	namespace RPC {
	
		template <typename ResultType, typename TokenType>
		class OperationData: public PushAPI<ResultType>, public RetrieveAPI<ResultType> {
			public:
				OperationData(HostAPI<ResultType, TokenType>& host, TokenType token)
					: host_(host), result_(boost::none), token_(token) {
						host_.add(token_, this);
					}
				
				~OperationData() {
					host_.remove(token_);
				}
				
				void operationComplete(ResultType&& result) {
					std::lock_guard<std::mutex> lock(mutex_);
					result_ = boost::optional<ResultType>(std::move(result));
					generator_.activate();
				}
				
				Event::Source eventSource() const {
					return generator_.eventSource();
				}
				
				bool isComplete() const {
					std::lock_guard<std::mutex> lock(mutex_);
					return static_cast<bool>(result_);
				}
				
				ResultType get() {
					std::lock_guard<std::mutex> lock(mutex_);
					if (!result_) {
						throw std::runtime_error("Result not available.");
					}
					ResultType result = std::move(*result_);
					result_ = boost::none;
					return result;
				}
				
			private:
				mutable std::mutex mutex_;
				Event::Generator generator_;
				HostAPI<ResultType, TokenType>& host_;
				boost::optional<ResultType> result_;
				TokenType token_;
			
		};
		
	}
	
}

#endif
