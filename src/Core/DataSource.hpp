#ifndef OPENP2P_CORE_DATASOURCE_HPP
#define OPENP2P_CORE_DATASOURCE_HPP

#include "Types.hpp"

namespace openp2p{

	namespace Core{

		class DataSource{
			public:
				virtual void Read(Uint8 *, unsigned int) = 0;

		};

	}

}

#endif
