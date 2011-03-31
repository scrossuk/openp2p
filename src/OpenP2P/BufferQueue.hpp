#ifndef OPENP2P_BUFFERQUEUE_HPP
#define OPENP2P_BUFFERQUEUE_HPP

#include <cstddef>
#include <stdint.h>
#include <queue>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/BufferIterator.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P {

	class BufferQueue: public Stream{
		public:
			BufferQueue();

			void add(const Buffer&);

			std::size_t readSome(uint8_t *, std::size_t);

			void cancel();

		private:
			BufferIterator iterator_;

			std::queue<Buffer> queue_;

	};

}

#endif
