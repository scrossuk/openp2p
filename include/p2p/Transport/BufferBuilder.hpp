#ifndef P2P_BUFFERBUILDER_HPP
#define P2P_BUFFERBUILDER_HPP

#include <stdint.h>
#include <cstddef>

#include <p2p/Transport/Transport_api.hpp>
#include <p2p/Transport/Stream.hpp>
#include <p2p/Transport/Buffer.hpp>

namespace p2p {

	class TRANSPORT_OPENP2P_API BufferBuilder: public OStream {
		public:
			BufferBuilder(Buffer&);
			
			bool isValid() const;
			
			Event::Source eventSource() const;
			
			size_t write(const uint8_t* data, size_t size);
			
		private:
			Buffer& buffer_;
			
	};
	
}

#endif
