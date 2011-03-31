#ifndef OPENP2P_BUFFERITERATOR_HPP
#define OPENP2P_BUFFERITERATOR_HPP

#include <stdint.h>
#include <cstddef>
#include <stack>
#include <boost/shared_ptr.hpp>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P {

	class BufferIterator: public Stream {
		public:
			BufferIterator();

			BufferIterator(const Buffer&, std::size_t = 0);

			std::size_t position();

			std::size_t remaining();

			void set(const Buffer&);

			bool seek(std::size_t);

			std::size_t readSome(uint8_t *, std::size_t);

			void cancel();

		private:
			bool moveSuccessor();

			Buffer buffer_;

			std::size_t position_, offset_;

			std::stack< boost::shared_ptr<BufferTree> > stack_;

	};

}

#endif
