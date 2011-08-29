#ifndef OPENP2P_OFTORRENT_XORSTREAM_HPP
#define OPENP2P_OFTORRENT_XORSTREAM_HPP

#include <OpenP2P/Block.hpp>
#include <OpenP2P/BufferedStream.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	namespace OFTorrent{

		/**
		 * A stream that reads from two streams, and xors them together.
		 */
		class XORIStream: public IStream{
			public:
				XORIStream(IStream& source0, IStream& source1);
				
				EventHandle readEvent();

				std::size_t readSome(uint8_t * data, std::size_t dataSize);

			private:
				BufferedStream bufferedStream_;
				IStream& stream_;

		};

	}

}

#endif
