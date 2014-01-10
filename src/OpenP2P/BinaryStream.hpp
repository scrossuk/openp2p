#ifndef OPENP2P_BINARYSTREAM_HPP
#define OPENP2P_BINARYSTREAM_HPP

#include <stdint.h>
#include <cstddef>

#include <OpenP2P/Stream.hpp>
#include <OpenP2P/Timeout.hpp>

namespace OpenP2P {

	class BinaryIStream {
		public:
			inline BinaryIStream(InputStream& stream) : stream_(stream), isValid_(true) { }
			
			inline bool isValid() {
				return isValid_;
			}
			
			inline void setValid() {
				isValid_ = true;
			}
			
			inline void setInvalid() {
				isValid_ = false;
			}
			
			// Read as much as possible - failing to read all the data is NOT an error.
			std::size_t tryRead(uint8_t* data, std::size_t size, Timeout timeout = Timeout::Infinite());
			
			// Read as much as possible - failing to read all the data is an error.
			bool read(uint8_t* data, std::size_t size, Timeout timeout = Timeout::Infinite());
			
			inline std::size_t waitForData(Timeout timeout = Timeout::Infinite()) {
				return stream_.waitForData(timeout);
			}
			
		private:
			InputStream& stream_;
			bool isValid_;
			
	};
	
	class BinaryOStream {
		public:
			inline BinaryOStream(OutputStream& stream) : stream_(stream), isValid_(true) { }
			
			inline bool isValid() {
				return isValid_;
			}
			
			inline void setValid() {
				isValid_ = true;
			}
			
			inline void setInvalid() {
				isValid_ = false;
			}
			
			// Write as much as possible - failing to write all the data is NOT an error.
			std::size_t tryWrite(const uint8_t* data, std::size_t size, Timeout timeout = Timeout::Infinite());
			
			// Write as much as possible - failing to write all the data is an error.
			bool write(const uint8_t* data, std::size_t size, Timeout timeout = Timeout::Infinite());
			
			inline std::size_t waitForSpace(Timeout timeout = Timeout::Infinite()) {
				return stream_.waitForSpace(timeout);
			}
			
		private:
			OutputStream& stream_;
			bool isValid_;
			
	};
	
	class BinaryIOStream {
		public:
			inline BinaryIOStream(IOStream& stream)
				: inputStream_(stream), outputStream_(stream) { }
				
			inline bool isValid() {
				return inputStream_.isValid() && outputStream_.isValid();
			}
			
			inline BinaryIStream& getInputStream() {
				return inputStream_;
			}
			
			inline BinaryOStream& getOutputStream() {
				return outputStream_;
			}
			
		private:
			BinaryIStream inputStream_;
			BinaryOStream outputStream_;
			
	};
	
	namespace Binary {
	
		bool ReadUint8(BinaryIStream& stream, uint8_t* value, Timeout timeout = Timeout::Infinite());
		
		bool ReadInt8(BinaryIStream& stream, int8_t* value, Timeout timeout = Timeout::Infinite());
		
		bool ReadUint16(BinaryIStream& stream, uint16_t* value, Timeout timeout = Timeout::Infinite());
		
		bool ReadInt16(BinaryIStream& stream, int16_t* value, Timeout timeout = Timeout::Infinite());
		
		bool ReadUint32(BinaryIStream& stream, uint32_t* value, Timeout timeout = Timeout::Infinite());
		
		bool ReadInt32(BinaryIStream& stream, int32_t* value, Timeout timeout = Timeout::Infinite());
		
		bool ReadUint64(BinaryIStream& stream, uint64_t* value, Timeout timeout = Timeout::Infinite());
		
		bool ReadInt64(BinaryIStream& stream, int64_t* value, Timeout timeout = Timeout::Infinite());
		
		
		bool WriteUint8(BinaryOStream& stream, uint8_t value, Timeout timeout = Timeout::Infinite());
		
		bool WriteInt8(BinaryOStream& stream, int8_t value, Timeout timeout = Timeout::Infinite());
		
		bool WriteUint16(BinaryOStream& stream, uint16_t value, Timeout timeout = Timeout::Infinite());
		
		bool WriteInt16(BinaryOStream& stream, int16_t value, Timeout timeout = Timeout::Infinite());
		
		bool WriteUint32(BinaryOStream& stream, uint32_t value, Timeout timeout = Timeout::Infinite());
		
		bool WriteInt32(BinaryOStream& stream, int32_t value, Timeout timeout = Timeout::Infinite());
		
		bool WriteUint64(BinaryOStream& stream, uint64_t value, Timeout timeout = Timeout::Infinite());
		
		bool WriteInt64(BinaryOStream& stream, int64_t value, Timeout timeout = Timeout::Infinite());
		
		
		std::size_t MoveData(BinaryIStream& source, BinaryOStream& destination, Timeout timeout = Timeout::Infinite());
		
	}
	
}

#endif
