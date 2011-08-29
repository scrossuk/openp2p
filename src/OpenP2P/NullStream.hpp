#ifndef OPENP2P_NULLSTREAM_HPP
#define OPENP2P_NULLSTREAM_HPP

#include <cstring>

#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	class NullIStream: public IStream{
		public:
			inline EventHandle readEvent(){
				return EventHandle::True;
			}
			
			inline std::size_t readSome(uint8_t * data, std::size_t dataSize){
				// Return zeroes.
				memset(data, 0, dataSize);
				return dataSize;
			}

	};

	class NullOStream: public OStream{
		public:
			inline EventHandle writeEvent(){
				return EventHandle::True;
			}
			
			inline std::size_t writeSome(const uint8_t * data, std::size_t dataSize){
				return dataSize;
			}

	};

	class NullIOStream: public NullIStream, public NullOStream{ };

}

#endif
