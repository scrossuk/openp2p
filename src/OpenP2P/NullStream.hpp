#ifndef OPENP2P_NULLSTREAM_HPP
#define OPENP2P_NULLSTREAM_HPP

#include <stdint.h>
#include <string.h>

#include <OpenP2P/Stream.hpp>

namespace OpenP2P {

	class NullIStream: public IStream {
		public:
			inline EventHandle readEvent() {
				return EventHandle::True;
			}
			
			inline bool isValid() const {
				return true;
			}
			
			inline size_t read(uint8_t* data, size_t size) {
				// Return zeroes.
				memset(data, 0, size);
				return size;
			}
			
	};
	
	class NullOStream: public OStream {
		public:
			inline bool isValid() const {
				return true;
			}
			
			inline size_t write(const uint8_t*, size_t size) {
				return size;
			}
			
	};
	
}

#endif
