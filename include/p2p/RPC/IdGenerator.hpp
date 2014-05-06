#ifndef P2P_RPC_IDGENERATOR_HPP
#define P2P_RPC_IDGENERATOR_HPP

namespace p2p {

	namespace RPC {
		
		template <typename IdType>
		class IdGenerator {
			public:
				virtual IdType generate() = 0;
				
		};
		
	}
	
}

#endif
