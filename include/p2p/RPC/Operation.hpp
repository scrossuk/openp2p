#ifndef P2P_RPC_OPERATION_HPP
#define P2P_RPC_OPERATION_HPP

#include <functional>
#include <memory>

#include <p2p/Event/Source.hpp>
#include <p2p/Event/Wait.hpp>

#include <p2p/RPC/ChainedOperationData.hpp>
#include <p2p/RPC/RetrieveAPI.hpp>

namespace p2p {

	namespace RPC {
	
		template <typename ResultType>
		class Operation {
			public:
				Operation(std::unique_ptr<RetrieveAPI<ResultType>>&& data)
					: data_(std::move(data)) { }
				
				Event::Source eventSource() const {
					return data_->eventSource();
				}
				
				bool isComplete() const {
					return data_->isComplete();
				}
				
				ResultType get() {
					wait();
					return data_->get();
				}
				
				void wait() const {
					while (!isComplete()) {
						Event::Wait({ eventSource() });
					}
				}
				
				std::unique_ptr<RetrieveAPI<ResultType>> detach() {
					return std::move(data_);
				}
				
			private:
				std::unique_ptr<RetrieveAPI<ResultType>> data_;
			
		};
		
		template <typename OriginalResultType, typename Operator>
		auto Chain(Operation<OriginalResultType> operation, Operator function) -> Operation<decltype(function(operation.get()))> {
			using ResultType = decltype(function(operation.get()));
			auto newSource = std::unique_ptr<RetrieveAPI<ResultType>>(
				new ChainedOperationData<ResultType, OriginalResultType, Operator>(std::move(operation.detach()), std::move(function)));
			return Operation<ResultType>(std::move(newSource));
		}
		
	}
	
}

#endif
