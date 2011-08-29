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
			
			EventHandle writeEvent();

			std::size_t writeSome(const uint8_t * data, std::size_t dataSize);

		private:
			Buffer& buffer_;

	};

}

#endif
