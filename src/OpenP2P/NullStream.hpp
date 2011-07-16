#ifndef OPENP2P_NULLSTREAM_HPP
#define OPENP2P_NULLSTREAM_HPP

#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	class NullIStream: public IStream{
		public:
			inline Future<std::size_t> readSome(uint8_t * data, std::size_t requestedSize){
				for(std::size_t i = 0; i < requestedSize; i++){
					data[i] = 0;
				}
				return requestedSize;
			}

	};

	class NullOStream: public OStream{
		public:
			inline Future<std::size_t> writeSome(const uint8_t * data, std::size_t requestedSize){
				return requestedSize;
			}

	};

	class NullIOStream: public NullIStream, public NullOStream{ };

}

#endif
