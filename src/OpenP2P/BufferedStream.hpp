#ifndef OPENP2P_BUFFEREDSTREAM_HPP
#define OPENP2P_BUFFEREDSTREAM_HPP

#include <stdint.h>
#include <cstddef>
#include <boost/scoped_array.hpp>

#include <OpenP2P/Future.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	const std::size_t DefaultBufferSize = 4096;

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
			 * @param bufferSize An optional parameter giving the size of the underlying buffer.
			 */
			BufferedStream(IStream& stream, std::size_t bufferSize = DefaultBufferSize);

			/**
			 * Reads as much data as possible into the buffer.
			 *
			 * @return The actual amount of data that was read. If this returns zero,
			 *         there is no data remaining in the underlying stream.
			 */
			inline Future<std::size_t> readSome(){
				return readSome(bufferSize_);
			}

			/**
			 * Reads some data from the stream into the buffer.
			 *
			 * @param requestedSize The amount of data to be read.
			 * @return The actual amount of data that was read. If this returns zero,
			 *         there is no data remaining in the underlying stream.
			 */
			Future<std::size_t> readSome(std::size_t requestedSize);
			
			/**
			 * Gets a pointer to data that has been read from the stream.
			 *
			 * @return A pointer to the data in the stream.
			 */
			inline const uint8_t * get(){
				return data_.get() + readPos_;
			}

			/**
			 * Gets the amount of data that has been read, but has not yet been consumed.
			 *
			 * @return The data size.
			 */
			inline std::size_t size(){
				// Update the total size.
				if(future_.isReady() && activeRead_){
					size_ = future_.get();
					activeRead_ = false;
				}
				
				return size_;
			}

			/**
			 * Gets the size of the buffer underlying the stream.
			 *
			 * @return The buffer size.
			 */
			inline std::size_t bufferSize() const{
				return bufferSize_;
			}

			/**
			 * Consume a certain amount of data that has been read, indicating it is
			 * no longer needed. Note that calling this invalidates the return value
			 * from any previous calls to getData().
			 *
			 * @param consumeSize The amount of data to be consumed.
			 */
			void consume(std::size_t consumeSize);

		private:		
			IStream& stream_;
			boost::scoped_array<uint8_t> data_;
			const std::size_t bufferSize_;
			std::size_t readPos_, size_;
			Future<std::size_t> future_;
			bool activeRead_;

	};

}

#endif
