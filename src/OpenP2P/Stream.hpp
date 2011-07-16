#ifndef OPENP2P_STREAM_HPP
#define OPENP2P_STREAM_HPP

#include <stdint.h>
#include <cstddef>

#include <OpenP2P/Future.hpp>

namespace OpenP2P{

	class IStream{
		public:
			virtual Future<std::size_t> readSome(uint8_t *, std::size_t) = 0;

	};

	class OStream{
		public:
			virtual Future<std::size_t> writeSome(const uint8_t *, std::size_t) = 0;

	};

	class IOStream: public IStream, public OStream{ };

}

#endif
