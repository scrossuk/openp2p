#ifndef P2P_RPC_HOSTAPI_HPP
#define P2P_RPC_HOSTAPI_HPP

#include <p2p/RPC/PushAPI.hpp>

namespace p2p {

	namespace RPC {
	
		template <typename ResultType, typename TokenType>
		class HostAPI {
			public:
				inline virtual ~HostAPI() { }
				
				virtual void add(const TokenType& token, PushAPI<ResultType>* pushData) = 0;
				
				virtual void remove(const TokenType& token) = 0;
			
		};
		
	}
	
}

#endif
