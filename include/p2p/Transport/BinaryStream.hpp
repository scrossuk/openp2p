#ifndef P2P_BINARYSTREAM_HPP
#define P2P_BINARYSTREAM_HPP

#include <stdint.h>
#include <cstddef>

#include <p2p/Transport/Transport_api.hpp>
#include <p2p/Transport/Stream.hpp>

namespace p2p {

	class TRANSPORT_OPENP2P_API BlockingReader {
		public:
			// Read as much as possible - failing to read all the data is an error.
			virtual void readAll(uint8_t* data, size_t size) = 0;
		
	};
	
	class TRANSPORT_OPENP2P_API BinaryIStream: public BlockingReader {
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
	
	class TRANSPORT_OPENP2P_API BlockingWriter {
		public:
			// Read as much as possible - failing to read all the data is an error.
			virtual void writeAll(const uint8_t* data, size_t size) = 0;
		
	};
	
	class TRANSPORT_OPENP2P_API BinaryOStream: public BlockingWriter {
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
	
	class TRANSPORT_OPENP2P_API BinaryIOStream {
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
	
		uint8_t TRANSPORT_OPENP2P_API ReadUint8(BlockingReader& stream);
		
		int8_t TRANSPORT_OPENP2P_API ReadInt8(BlockingReader& stream);
		
		uint16_t TRANSPORT_OPENP2P_API ReadUint16(BlockingReader& stream);
		
		int16_t TRANSPORT_OPENP2P_API ReadInt16(BlockingReader& stream);
		
		uint32_t TRANSPORT_OPENP2P_API ReadUint32(BlockingReader& stream);
		
		int32_t TRANSPORT_OPENP2P_API ReadInt32(BlockingReader& stream);
		
		uint64_t TRANSPORT_OPENP2P_API ReadUint64(BlockingReader& stream);
		
		int64_t TRANSPORT_OPENP2P_API ReadInt64(BlockingReader& stream);
		
		
		void TRANSPORT_OPENP2P_API WriteUint8(BlockingWriter& stream, uint8_t value);
		
		void TRANSPORT_OPENP2P_API WriteInt8(BlockingWriter& stream, int8_t value);
		
		void TRANSPORT_OPENP2P_API WriteUint16(BlockingWriter& stream, uint16_t value);
		
		void TRANSPORT_OPENP2P_API WriteInt16(BlockingWriter& stream, int16_t value);
		
		void TRANSPORT_OPENP2P_API WriteUint32(BlockingWriter& stream, uint32_t value);
		
		void TRANSPORT_OPENP2P_API WriteInt32(BlockingWriter& stream, int32_t value);
		
		void TRANSPORT_OPENP2P_API WriteUint64(BlockingWriter& stream, uint64_t value);
		
		void TRANSPORT_OPENP2P_API WriteInt64(BlockingWriter& stream, int64_t value);
		
		
		void TRANSPORT_OPENP2P_API MoveData(IStream& source, OStream& destination);
		
	}
	
}

#endif
