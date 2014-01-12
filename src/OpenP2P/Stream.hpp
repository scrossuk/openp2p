#ifndef OPENP2P_STREAM_HPP
#define OPENP2P_STREAM_HPP

#include <stdint.h>

#include <OpenP2P/Event/Source.hpp>

namespace OpenP2P {
	
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
