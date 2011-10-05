#ifndef OPENP2P_NULLSTREAM_HPP
#define OPENP2P_NULLSTREAM_HPP

#include <cstring>
#include <limits>

#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	class NullIStream: public InputStream{
		public:
			inline EventHandle readEvent(){
				return EventHandle::True;
			}
			
			inline std::size_t waitForData(Timeout){
				return std::numeric_limits<std::size_t>::max();
			}
			
			inline bool read(uint8_t * data, std::size_t size, Timeout){
				// Return zeroes.
				memset(data, 0, size);
				return true;
			}

	};

	class NullOStream: public OutputStream{
		public:
			inline std::size_t waitForSpace(Timeout){
				return std::numeric_limits<std::size_t>::max();
			}
			
			inline bool write(const uint8_t * data, std::size_t size, Timeout){
				return true;
			}

	};

	class NullIOStream: public NullIStream, public NullOStream{ };

}

#endif
