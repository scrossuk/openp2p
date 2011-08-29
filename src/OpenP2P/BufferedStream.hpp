#ifndef OPENP2P_BUFFEREDSTREAM_HPP
#define OPENP2P_BUFFEREDSTREAM_HPP

#include <stdint.h>
#include <cstddef>

#include <boost/scoped_array.hpp>

#include <OpenP2P/EventDispatcher.hpp>
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
			static const std::size_t DefaultBufferSize = 4096;
		
			/**
			 * Constructs a buffered stream on top of an input stream.
			 *
			 * @param stream The stream from which data is to be read.
			 */
			BufferedStream(IStream& stream, std::size_t bufferSize = DefaultBufferSize);
			
			/**
			 * Returns an event handle that is active when there are read events.
			 *
			 * @return An event handle for read events.
			 */
			EventHandle readEvent();
			
			/**
			 * Gets a pointer to the start of the internal buffer.
			 *
			 * @return Pointer to internal buffer.
			 */
			inline const uint8_t * get() const{
				return data_.get();
			}
			
			/**
			 * Gets a byte from the internal buffer at the position given.
			 *
			 * @param pos Position in internal buffer to read.
			 * @return Byte in internal buffer at position given.
			 */
			inline const uint8_t& operator[](std::size_t pos) const{
				return data_[pos];
			}
			
			/**
			 * Reads as much data as possible from the underlying stream to the internal buffer.
			 *
			 * @return Amount of data now in the internal buffer (0 if underlying stream has no more data).
			 */
			inline std::size_t read(){
				return read(bufferSize_);
			}

			/**
			 * Reads some data from the underlying stream to the internal buffer. Note that
			 * no reading will occur if the internal buffer already holds the amount of
			 * data specified.
			 *
			 * @param dataSize Amount of data to read into internal buffer.
			 * @return Amount of data now in the internal buffer (0 if underlying stream has no more data).
			 */
			std::size_t read(std::size_t dataSize);

			/**
			 * Consume a certain amount of data that has been read, indicating it is
			 * no longer needed.
			 *
			 * @param consumeSize The amount of data to be consumed.
			 */
			void consume(std::size_t consumeSize);
			
			/**
			 * Gets the amount of data that has been read, but has not yet been consumed.
			 *
			 * @return The data size.
			 */
			inline std::size_t size() const{
				return writePos_ - readPos_;
			}
			
			/**
			 * Gets the capacity of the internal buffer.
			 *
			 * @return The internal buffer's capacity.
			 */
			inline std::size_t capacity() const{
				return bufferSize_ - writePos_;
			}
			
			/**
			 * Gets the size of the internal buffer.
			 *
			 * @return The internal buffer's size.
			 */
			inline std::size_t bufferSize() const{
				return bufferSize_;
			}

		private:		
			IStream& stream_;
			EventDispatcher dispatcher_;
			boost::scoped_array<uint8_t> data_;
			const std::size_t bufferSize_;
			std::size_t readPos_, writePos_;

	};

}

#endif
