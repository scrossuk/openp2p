#ifndef OPENP2P_STREAM_HPP
#define OPENP2P_STREAM_HPP

#include <stdint.h>
#include <cstddef>

#include <OpenP2P/EventHandle.hpp>

namespace OpenP2P{

	class IStream{
		public:
			virtual EventHandle readEvent() = 0;
			
			virtual std::size_t readSome(uint8_t * data, std::size_t dataSize) = 0;

	};

	class OStream{
		public:
			virtual EventHandle writeEvent() = 0;
		
			virtual std::size_t writeSome(const uint8_t * data, std::size_t dataSize) = 0;

	};

	class IOStream: public IStream, public OStream{ };

}

#endif
