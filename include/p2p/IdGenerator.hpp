#ifndef P2P_IDGENERATOR_HPP
#define P2P_IDGENERATOR_HPP

namespace p2p {

	template <typename IdType>
	class IdGenerator {
		public:
			virtual IdType generate() = 0;
			
	};
	
}

#endif
