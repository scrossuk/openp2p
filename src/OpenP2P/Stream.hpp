#ifndef OPENP2P_STREAM_HPP
#define OPENP2P_STREAM_HPP

#include <stdint.h>
#include <cstddef>

#include <OpenP2P/Timeout.hpp>

namespace OpenP2P {

	class InputStream {
		public:
			virtual std::size_t waitForData(Timeout timeout) = 0;
			
			virtual bool read(uint8_t* data, std::size_t size, Timeout timeout) = 0;
			
	};
	
	class OutputStream {
		public:
			virtual std::size_t waitForSpace(Timeout timeout) = 0;
			
			virtual bool write(const uint8_t* data, std::size_t size, Timeout timeout) = 0;
			
	};
	
	class IOStream: public InputStream, public OutputStream { };
	
}

#endif
