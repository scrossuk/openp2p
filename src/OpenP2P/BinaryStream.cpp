#include <stdint.h>
#include <algorithm>
#include <cstddef>
#include <cstring>

#include <OpenP2P/BinaryStream.hpp>
#include <OpenP2P/Stream.hpp>
#include <OpenP2P/Timeout.hpp>
#include <OpenP2P/TimeoutSequence.hpp>

namespace OpenP2P {

	bool BinaryIStream::read(uint8_t* data, std::size_t size, Timeout timeout) {
		isValid_ = (tryRead(data, size, timeout) == size);
		
		if (!isValid_) {
			memset((void*) data, 0, size);
		}
		
		return isValid_;
	}
	
	std::size_t BinaryIStream::tryRead(uint8_t* data, std::size_t size, Timeout timeout) {
		TimeoutSequence sequence(timeout);
		
		std::size_t pos = 0;
		
		while (pos < size) {
			const std::size_t availableDataSize = stream_.waitForData(sequence.getTimeout());
			
			if (availableDataSize == 0) {
				return pos;
			}
			
			const std::size_t readSize = std::min(availableDataSize, size - pos);
			
			if (readSize == 0) {
				return pos;
			}
			
			if (!stream_.read(data + pos, readSize, sequence.getTimeout())) {
				return pos;
			}
			
			pos += readSize;
		}
		
		return pos;
	}
	
	bool BinaryOStream::write(const uint8_t* data, std::size_t size, Timeout timeout) {
		return isValid_ = (tryWrite(data, size, timeout) == size);
	}
	
	std::size_t BinaryOStream::tryWrite(const uint8_t* data, std::size_t size, Timeout timeout) {
		TimeoutSequence sequence(timeout);
		
		std::size_t pos = 0;
		
		while (pos < size) {
			const std::size_t availableCapacitySize = stream_.waitForSpace(sequence.getTimeout());
			
			if (availableCapacitySize == 0) {
				return pos;
			}
			
			const std::size_t writeSize = std::min(availableCapacitySize, size - pos);
			
			if (writeSize == 0) {
				return pos;
			}
			
			if (!stream_.write(data + pos, writeSize, sequence.getTimeout())) {
				return pos;
			}
			
			pos += writeSize;
		}
		
		return pos;
	}
	
	namespace Binary {
	
		bool ReadUint8(BinaryIStream& stream, uint8_t* value, Timeout timeout) {
			return stream.read(value, 1, timeout);
		}
		
		bool ReadInt8(BinaryIStream& stream, int8_t* value, Timeout timeout) {
			return ReadUint8(stream, (uint8_t*) value, timeout);
		}
		
		bool ReadUint16(BinaryIStream& stream, uint16_t* value, Timeout timeout) {
			uint8_t s[2];
			bool success = stream.read(s, 2, timeout);
			*value = (s[0] << 8) | s[1];
			return success;
		}
		
		bool ReadInt16(BinaryIStream& stream, int16_t* value, Timeout timeout) {
			return ReadUint16(stream, (uint16_t*) value, timeout);
		}
		
		bool ReadUint32(BinaryIStream& stream, uint32_t* value, Timeout timeout) {
			uint8_t s[4];
			bool success = stream.read(s, 4, timeout);
			*value = (uint32_t(s[0]) << 24) | (uint32_t(s[1]) << 16) | (uint32_t(s[2]) << 8) | uint32_t(s[3]);
			return success;
		}
		
		bool ReadInt32(BinaryIStream& stream, int32_t* value, Timeout timeout) {
			return ReadUint32(stream, (uint32_t*) value, timeout);
		}
		
		bool ReadUint64(BinaryIStream& stream, uint64_t* value, Timeout timeout) {
			uint8_t s[8];
			bool success = stream.read(s, 8, timeout);
			*value = (uint64_t(s[0]) << 56) | (uint64_t(s[1]) << 48) | (uint64_t(s[2]) << 40) | (uint64_t(s[3]) << 32)
					 | (uint64_t(s[4]) << 24) | (uint64_t(s[5]) << 16) | (uint64_t(s[6]) << 8) | uint64_t(s[7]);
			return success;
		}
		
		bool ReadInt64(BinaryIStream& stream, int64_t* value, Timeout timeout) {
			return ReadUint64(stream, (uint64_t*) value, timeout);
		}
		
		
		bool WriteUint8(BinaryOStream& stream, uint8_t value, Timeout timeout) {
			return stream.write(&value, 1, timeout);
		}
		
		bool WriteInt8(BinaryOStream& stream, int8_t value, Timeout timeout) {
			return WriteUint8(stream, value, timeout);
		}
		
		bool WriteUint16(BinaryOStream& stream, uint16_t value, Timeout timeout) {
			uint8_t s[2];
			s[0] = (value >> 8) & 0xFF;
			s[1] = value & 0xFF;
			return stream.write(s, 2, timeout);
		}
		
		bool WriteInt16(BinaryOStream& stream, int16_t value, Timeout timeout) {
			return WriteUint16(stream, value, timeout);
		}
		
		bool WriteUint32(BinaryOStream& stream, uint32_t value, Timeout timeout) {
			uint8_t s[4];
			s[0] = (value >> 24) & 0xFF;
			s[1] = (value >> 16) & 0xFF;
			s[2] = (value >> 8) & 0xFF;
			s[3] = value & 0xFF;
			return stream.write(s, 4, timeout);
		}
		
		bool WriteInt32(BinaryOStream& stream, int32_t value, Timeout timeout) {
			return WriteUint32(stream, value, timeout);
		}
		
		bool WriteUint64(BinaryOStream& stream, uint64_t value, Timeout timeout) {
			uint8_t s[8];
			s[0] = (value >> 56) & 0xFF;
			s[1] = (value >> 48) & 0xFF;
			s[2] = (value >> 40) & 0xFF;
			s[3] = (value >> 32) & 0xFF;
			s[4] = (value >> 24) & 0xFF;
			s[5] = (value >> 16) & 0xFF;
			s[6] = (value >> 8) & 0xFF;
			s[7] = value & 0xFF;
			return stream.write(s, 8, timeout);
		}
		
		bool WriteInt64(BinaryOStream& stream, int64_t value, Timeout timeout) {
			return WriteUint64(stream, value, timeout);
		}
		
		
		std::size_t MoveData(BinaryIStream& source, BinaryOStream& destination, Timeout timeout) {
			TimeoutSequence sequence(timeout);
			
			const std::size_t dataSize = 256;
			uint8_t data[dataSize];
			std::size_t dataMoved = 0;
			
			while (true) {
				const std::size_t space = destination.waitForSpace(sequence.getTimeout());
				
				if (space == 0) {
					return dataMoved;
				}
				
				const std::size_t transferSize = std::min(space, dataSize);
				
				const std::size_t readSize = source.tryRead(data, transferSize, sequence.getTimeout());
				
				if (readSize == 0) {
					return dataMoved;
				}
				
				if (!destination.write(data, readSize, sequence.getTimeout())) {
					return dataMoved;
				}
				
				dataMoved += readSize;
			}
		}
		
	}
	
}

