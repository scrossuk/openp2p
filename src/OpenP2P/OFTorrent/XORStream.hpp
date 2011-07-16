#ifndef OPENP2P_OFTORRENT_XORSTREAM_HPP
#define OPENP2P_OFTORRENT_XORSTREAM_HPP

#include <stdint.h>
#include <cstddef>
#include <boost/scoped_array.hpp>
#include <OpenP2P/BufferedStream.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	namespace OFTorrent{

		/**
		 * A stream that reads from two streams, and xors them together.
		 */
		class XORIStream: public IStream{
			public:
				XORIStream(IStream& source1, IStream& source2);

				Future<std::size_t> readSome(uint8_t *, std::size_t);

			private:
				std::size_t onRead(uint8_t * data, std::pair<std::size_t, std::size_t> readSizes);
			
				BufferedStream buffered1_;
				BufferedStream buffered2_;

		};

	}

}

#endif
