#ifndef P2P_RPC_OPERATION_HPP
#define P2P_RPC_OPERATION_HPP

#include <memory>

#include <p2p/Event/Source.hpp>
#include <p2p/Event/Wait.hpp>

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
				
				const ResultType& get() const {
					return data_->get();
				}
				
				ResultType move() {
					return data_->move();
				}
				
				ResultType wait() {
					while (!isComplete()) {
						Event::Wait({ eventSource() });
					}
					return move();
				}
				
			private:
				std::unique_ptr<RetrieveAPI<ResultType>> data_;
			
		};
		
	}
	
}

#endif
