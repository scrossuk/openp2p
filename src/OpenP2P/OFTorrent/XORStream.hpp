#ifndef OPENP2P_OFTORRENT_XORSTREAM_HPP
#define OPENP2P_OFTORRENT_XORSTREAM_HPP

#include <OpenP2P/Block.hpp>
#include <OpenP2P/BufferedStream.hpp>
#include <OpenP2P/Future.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	namespace OFTorrent{

		/**
		 * A stream that reads from two streams, and xors them together.
		 */
		class XORIStream: public IStream{
			public:
				XORIStream(IStream& source0, IStream& source1);

				Future<Block> readSome();

			private:
				BufferedStream buffered0_;
				BufferedStream buffered1_;
				Future<Block> future_;

		};

	}

}

#endif
