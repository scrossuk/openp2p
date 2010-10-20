#ifndef OPENP2P_CORE_SERIALIZABLE_HPP
#define OPENP2P_CORE_SERIALIZABLE_HPP

#include "Buffer.hpp"

namespace openp2p{

	namespace Core{

		class Serializable{
			public:
				virtual Buffer Serialize() = 0;

				virtual ~Serializable(){ }

		};

	}

}

#endif
