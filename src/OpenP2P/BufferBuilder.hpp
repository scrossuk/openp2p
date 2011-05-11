#ifndef OPENP2P_BUFFERBUILDER_HPP
#define OPENP2P_BUFFERBUILDER_HPP

#include <stdint.h>
#include <cstddef>
#include <vector>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	class BufferBuilder: public OStream{
		public:
			BufferBuilder(Buffer&);

			std::size_t writeSome(const uint8_t *, std::size_t);

			void cancel();

		private:
			Buffer& buffer_;

	};

}

#endif
