#ifndef P2P_RPC_CHAINEDOPERATIONDATA_HPP
#define P2P_RPC_CHAINEDOPERATIONDATA_HPP

#include <functional>
#include <memory>

#include <p2p/Event/Source.hpp>

#include <p2p/RPC/RetrieveAPI.hpp>

namespace p2p {

	namespace RPC {
		
		template <typename ResultType, typename OriginalResultType, typename Operator>
		class ChainedOperationData: public RetrieveAPI<ResultType> {
			public:
				ChainedOperationData(std::unique_ptr<RetrieveAPI<OriginalResultType>>&& originalSource, Operator function)
					: originalSource_(std::move(originalSource)),
					function_(std::move(function)) { }
				
				Event::Source eventSource() const {
					return originalSource_->eventSource();
				}
				
				bool isComplete() const {
					return originalSource_->isComplete();
				}
				
				ResultType get() {
					return function_(originalSource_->get());
				}
				
			private:
				std::unique_ptr<RetrieveAPI<OriginalResultType>> originalSource_;
				Operator function_;
				
		};
		
	}
	
}

#endif
