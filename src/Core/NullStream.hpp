#ifndef OPENP2P_CORE_NULLSTREAM_HPP
#define OPENP2P_CORE_NULLSTREAM_HPP

#include "Stream.hpp"
#include "Buffer.hpp"

namespace openp2p{

	namespace Core{

		class NullStream: public Stream{
			public:
				NullStream();

				void Data(Buffer);

		};

	}

}

#endif
