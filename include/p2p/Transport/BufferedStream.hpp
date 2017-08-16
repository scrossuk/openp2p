#ifndef P2P_BUFFEREDSTREAM_HPP
#define P2P_BUFFEREDSTREAM_HPP

#include <stdint.h>
#include <cstddef>

#include <boost/scoped_array.hpp>

#include <p2p/Transport/Transport_api.hpp>
#include <p2p/Transport/Stream.hpp>

namespace p2p {

	/**
	 * This class provides a way to read data from a stream without consuming it immediately.
	 * It can be useful when reading from two streams: since one stream could produce a
	 * smaller read than the other it is necessary to keep the data from the larger read.
	 * Note that, as with all streams, instances of this class are NOT thread safe.
	 */
	class TRANSPORT_OPENP2P_API BufferedStream {
		public:
			static const size_t DefaultBufferSize = 4096;
			
			/**
			 * Constructs a buffered stream on top of an input stream.
			 *
			 * @param stream The stream from which data is to be read.
			 * @param bufferSize Size of the internal buffer.
			 */
			BufferedStream(IStream& stream, size_t bufferSize = DefaultBufferSize);
			
			/**
			 * Query whether the stream is valid (i.e. read() operations
			 * can still be performed on the stream).
			 * 
			 * @return Whether the stream is valid.
			 */
			inline bool isValid() const {
				return size() > 0 || stream_.isValid();
			}
			
			/**
			 * Get the underlying stream's event source.
			 * 
			 * @return The event source.
			 */
			inline Event::Source eventSource() const {
				return stream_.eventSource();
			}
			
			/**
			 * Gets a pointer to the start of the internal buffer.
			 *
			 * @return Pointer to internal buffer.
			 */
			inline const uint8_t* get() const {
				return data_.get();
			}
			
			/**
			 * Gets a byte from the internal buffer at the position given.
			 *
			 * @param pos Position in internal buffer to read.
			 * @return Byte in internal buffer at position given.
			 */
			inline uint8_t operator[](size_t pos) const {
				return data_[pos];
			}
			
			/**
			 * Reads as much data as possible from the underlying stream to the internal buffer.
			 * 
			 * @return Amount of data now in the internal buffer.
			 */
			inline size_t readMax() {
				return read(bufferSize_);
			}
			
			/**
			 * Reads some data from the underlying stream to the internal buffer. Note that
			 * no reading will occur if the internal buffer already holds the amount of
			 * data specified.
			 *
			 * @param dataSize Amount of data to read into internal buffer.
			 * @return Amount of data now in the internal buffer.
			 */
			size_t read(size_t dataSize);
			
			/**
			 * Consume a certain amount of data that has been read, indicating it is
			 * no longer needed.
			 *
			 * @param consumeSize The amount of data to be consumed.
			 */
			void consume(size_t consumeSize);
			
			/**
			 * Gets the amount of data that has been read, but has not yet been consumed.
			 *
			 * @return The data size.
			 */
			inline size_t size() const {
				return writePos_ - readPos_;
			}
			
			/**
			 * Gets the capacity of the internal buffer.
			 *
			 * @return The internal buffer's capacity.
			 */
			inline size_t capacity() const {
				return bufferSize_ - writePos_;
			}
			
			/**
			 * Gets the size of the internal buffer.
			 *
			 * @return The internal buffer's size.
			 */
			inline size_t bufferSize() const {
				return bufferSize_;
			}
			
		private:
			IStream& stream_;
			boost::scoped_array<uint8_t> data_;
			const size_t bufferSize_;
			size_t readPos_, writePos_;
			
	};
	
}

#endif
