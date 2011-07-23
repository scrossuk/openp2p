#ifndef OPENP2P_BUFFEREDSTREAM_HPP
#define OPENP2P_BUFFEREDSTREAM_HPP

#include <cstddef>

#include <OpenP2P/Block.hpp>
#include <OpenP2P/Future.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	/**
	 * This class provides a way to read data from a stream without consuming it immediately.
	 * It can be useful when reading from two streams: since one stream could produce a
	 * smaller read than the other it is necessary to keep the data from the larger read.
	 * Note that, as with all streams, instances of this class are NOT thread safe.
	 */
	class BufferedStream{
		public:
			/**
			 * Constructs a buffered stream on top of an input stream.
			 *
			 * @param stream The stream from which data is to be read.
			 */
			BufferedStream(IStream& stream);

			/**
			 * Read the block in the stream buffer.
			 *
			 * @return A future that resolves to the buffered block.
			 */
			Future<Block> readSome();

			/**
			 * Consume a certain amount of data that has been read, indicating it is
			 * no longer needed.
			 *
			 * @param consumeSize The amount of data to be consumed.
			 */
			void consume(std::size_t consumeSize);

		private:		
			IStream& stream_;
			Block block_;
			bool activeRead_;
			Future<Block> future_;

	};

}

#endif
