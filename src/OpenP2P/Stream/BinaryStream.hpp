#ifndef OPENP2P_BINARYSTREAM_HPP
#define OPENP2P_BINARYSTREAM_HPP

#include <stdint.h>
#include <cstddef>

#include <OpenP2P/Stream.hpp>

namespace OpenP2P {

	class BlockingReader {
		public:
			// Read as much as possible - failing to read all the data is an error.
			virtual void readAll(uint8_t* data, size_t size) = 0;
		
	};
	
	class BinaryIStream: public BlockingReader {
		public:
			inline BinaryIStream(IStream& stream) : stream_(stream) { }
			
			inline bool isValid() {
				return stream_.isValid();
			}
			
			// Read as much as possible - failing to read all the data is NOT an error.
			size_t readSome(uint8_t* data, size_t size);
			
			// Read as much as possible - failing to read all the data is an error.
			void readAll(uint8_t* data, size_t size);
			
		private:
			IStream& stream_;
			
	};
	
	class BlockingWriter {
		public:
			// Read as much as possible - failing to read all the data is an error.
			virtual void writeAll(const uint8_t* data, size_t size) = 0;
		
	};
	
	class BinaryOStream: public BlockingWriter {
		public:
			inline BinaryOStream(OStream& stream) : stream_(stream) { }
			
			inline bool isValid() {
				return stream_.isValid();
			}
			
			// Write as much as possible - failing to write all the data is NOT an error.
			size_t writeSome(const uint8_t* data, size_t size);
			
			// Write as much as possible - failing to write all the data is an error.
			void writeAll(const uint8_t* data, size_t size);
			
		private:
			OStream& stream_;
			
	};
	
	class BinaryIOStream {
		public:
			inline BinaryIOStream(IOStream& stream)
				: inputStream_(stream), outputStream_(stream) { }
				
			inline bool isValid() {
				return inputStream_.isValid() && outputStream_.isValid();
			}
			
			inline BinaryIStream& input() {
				return inputStream_;
			}
			
			inline BinaryOStream& output() {
				return outputStream_;
			}
			
		private:
			BinaryIStream inputStream_;
			BinaryOStream outputStream_;
			
	};
	
	namespace Binary {
	
		uint8_t ReadUint8(BlockingReader& stream);
		
		int8_t ReadInt8(BlockingReader& stream);
		
		uint16_t ReadUint16(BlockingReader& stream);
		
		int16_t ReadInt16(BlockingReader& stream);
		
		uint32_t ReadUint32(BlockingReader& stream);
		
		int32_t ReadInt32(BlockingReader& stream);
		
		uint64_t ReadUint64(BlockingReader& stream);
		
		int64_t ReadInt64(BlockingReader& stream);
		
		
		void WriteUint8(BlockingWriter& stream, uint8_t value);
		
		void WriteInt8(BlockingWriter& stream, int8_t value);
		
		void WriteUint16(BlockingWriter& stream, uint16_t value);
		
		void WriteInt16(BlockingWriter& stream, int16_t value);
		
		void WriteUint32(BlockingWriter& stream, uint32_t value);
		
		void WriteInt32(BlockingWriter& stream, int32_t value);
		
		void WriteUint64(BlockingWriter& stream, uint64_t value);
		
		void WriteInt64(BlockingWriter& stream, int64_t value);
		
		
		void MoveData(IStream& source, OStream& destination);
		
	}
	
}

#endif
