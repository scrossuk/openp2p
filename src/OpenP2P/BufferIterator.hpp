#ifndef OPENP2P_BUFFERITERATOR_HPP
#define OPENP2P_BUFFERITERATOR_HPP

#include <stdint.h>
#include <cstddef>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P {

	class BufferIterator: public InputStream {
		public:
			BufferIterator(const Buffer& buffer, std::size_t position = 0);

			std::size_t position();

			std::size_t remaining();

			void set(const Buffer&);

			bool seek(std::size_t position);
			
			std::size_t waitForData(Timeout);

			bool read(uint8_t * data, std::size_t size, Timeout);

		private:
			Buffer buffer_;
			std::size_t position_;

	};

}

#endif
