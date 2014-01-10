#ifndef OPENP2P_IDGENERATOR_HPP
#define OPENP2P_IDGENERATOR_HPP

namespace OpenP2P {

	template <typename IdType>
	class IdGenerator {
		public:
			virtual IdType generate() = 0;
			
	};
	
}

#endif
