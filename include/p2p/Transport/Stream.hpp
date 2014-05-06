#ifndef P2P_STREAM_HPP
#define P2P_STREAM_HPP

#include <stdint.h>

#include <p2p/Event/Source.hpp>

namespace p2p {
	
	class BasicStream {
		public:
			virtual bool isValid() const = 0;
			
			virtual Event::Source eventSource() const = 0;
			
	};
	
	class IStream: public virtual BasicStream {
		public:
			virtual size_t read(uint8_t* data, size_t size) = 0;
			
	};
	
	class OStream: public virtual BasicStream {
		public:
			virtual size_t write(const uint8_t* data, size_t size) = 0;
			
	};
	
	class IOStream: public IStream, public OStream { };
	
}

#endif
