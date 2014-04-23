#ifndef P2P_RPC_PUSHAPI_HPP
#define P2P_RPC_PUSHAPI_HPP

namespace p2p {

	namespace RPC {
	
		template <typename ResultType>
		class PushAPI {
			public:
				inline virtual ~PushAPI() { }
				
				virtual void operationComplete(ResultType&& result) = 0;
			
		};
		
	}
	
}

#endif
