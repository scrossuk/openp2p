#ifndef P2P_RPC_RETRIEVEAPI_HPP
#define P2P_RPC_RETRIEVEAPI_HPP

#include <p2p/Event/Source.hpp>

namespace p2p {

	namespace RPC {
	
		template <typename ResultType>
		class RetrieveAPI {
			public:
				inline virtual ~RetrieveAPI() { }
				
				virtual Event::Source eventSource() const = 0;
				
				virtual bool isComplete() const = 0;
				
				virtual ResultType get() = 0;
				
		};
		
	}
	
}

#endif
