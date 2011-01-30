#ifndef OPENP2P_GENERATOR_HPP
#define OPENP2P_GENERATOR_HPP

namespace OpenP2P{

	template <typename IdType>
	class Generator{
		public:
			virtual IdType generate() = 0;
	
	};

}

#endif
