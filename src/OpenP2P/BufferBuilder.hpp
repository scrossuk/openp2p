#ifndef OPENP2P_BUFFERBUILDER_HPP
#define OPENP2P_BUFFERBUILDER_HPP

#include <stdint.h>
#include <cstddef>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P {

	class BufferBuilder: public OStream {
		public:
			BufferBuilder(Buffer&);
			
			bool isValid() const;
			
			size_t write(const uint8_t* data, size_t size);
			
		private:
			Buffer& buffer_;
			
	};
	
}

#endif
