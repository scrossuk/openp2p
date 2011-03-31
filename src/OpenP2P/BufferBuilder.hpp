#ifndef OPENP2P_BUFFERBUILDER_HPP
#define OPENP2P_BUFFERBUILDER_HPP

#include <stdint.h>
#include <cstddef>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	class BufferBuilder: public Stream{
		public:
			BufferBuilder();

			std::size_t size();

			std::size_t writeSome(const uint8_t *, std::size_t);

			void cancel();

			Buffer getBuffer() const;

		private:
			Buffer buffer_;

	};

}

#endif
