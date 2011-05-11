#ifndef OPENP2P_STREAM_HPP
#define OPENP2P_STREAM_HPP

#include <stdint.h>
#include <cstddef>

#include <OpenP2P/Cancellable.hpp>

namespace OpenP2P{

	class IStream: public virtual Cancellable{
		public:
			virtual std::size_t readSome(uint8_t *, std::size_t) = 0;

	};

	class OStream: public virtual Cancellable{
		public:
			virtual std::size_t writeSome(const uint8_t *, std::size_t) = 0;

	};

	class IOStream: public IStream, public OStream{ };

}

#endif
