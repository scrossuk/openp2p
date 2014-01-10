#ifndef OPENP2P_BUFFERBUILDER_HPP
#define OPENP2P_BUFFERBUILDER_HPP

#include <stdint.h>
#include <cstddef>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P {

	class BufferBuilder: public OutputStream {
		public:
			BufferBuilder(Buffer&);
			
			std::size_t waitForSpace(Timeout);
			
			bool write(const uint8_t* data, std::size_t size, Timeout);
			
		private:
			Buffer& buffer_;
			
	};
	
}

#endif
