#ifndef OPENP2P_BUFFERITERATOR_HPP
#define OPENP2P_BUFFERITERATOR_HPP

#include <stdint.h>

#include <OpenP2P/Stream.hpp>

#include <OpenP2P/Buffer/Buffer.hpp>

namespace OpenP2P {

	class BufferIterator: public IStream {
		public:
			BufferIterator(const Buffer& buffer, size_t position = 0);
			
			size_t position() const;
			
			size_t remaining() const;
			
			void set(const Buffer&);
			
			bool seek(size_t position);
			
			bool isValid() const;
			
			Event::Source eventSource() const;
			
			size_t read(uint8_t* data, size_t size);
			
		private:
			Buffer buffer_;
			size_t position_;
			
	};
	
}

#endif
