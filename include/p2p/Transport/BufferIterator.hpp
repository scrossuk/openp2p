#ifndef P2P_BUFFERITERATOR_HPP
#define P2P_BUFFERITERATOR_HPP

#include <stdint.h>

#include <p2p/Transport/Transport_api.hpp>
#include <p2p/Transport/Stream.hpp>
#include <p2p/Transport/Buffer.hpp>

namespace p2p {

	class TRANSPORT_OPENP2P_API BufferIterator: public IStream {
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
