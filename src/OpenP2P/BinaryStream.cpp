#include <stdint.h>
#include <cstddef>
#include <cstring>
#include <string>

#include <OpenP2P/BinaryStream.hpp>
#include <OpenP2P/BufferBuilder.hpp>
#include <OpenP2P/BufferIterator.hpp>

namespace OpenP2P {

	BinaryStream::BinaryStream(Stream& stream) : stream_(stream), isValid_(true){ }

	bool BinaryStream::read(uint8_t * data, std::size_t length) {
		std::size_t p = 0;
		while (p < length) {
			std::size_t readSize = stream_.readSome(data + p, length - p);
			if (readSize == 0) {
				//Zero out data if the read fails
				memset(data, 0, length);
				isValid_ = false;
				return false;
			}
			else {
				p += readSize;
			}
		}
		isValid_ = true;
		return true;
	}


	bool BinaryStream::write(const uint8_t * data, std::size_t length) {
		std::size_t p = 0;
		while (p < length) {
			std::size_t writeSize = stream_.writeSome(data + p, length - p);
			if (writeSize == 0) {
				isValid_ = false;
				return false;
			}
			else {
				p += writeSize;
			}
		}
		isValid_ = true;
		return true;
	}

	std::size_t BinaryStream::tryRead(uint8_t * data, std::size_t length) {
		std::size_t p = 0;
		while (p < length) {
			std::size_t readSize = stream_.readSome(data + p, length - p);
			p += readSize;
			if (readSize == 0) break;
		}
		return p;
	}


	std::size_t BinaryStream::tryWrite(const uint8_t * data, std::size_t length) {
		std::size_t p = 0;
		while (p < length) {
			std::size_t writeSize = stream_.writeSome(data + p, length - p);
			p += writeSize;
			if (writeSize == 0) break;
		}
		return p;
	}

	void BinaryStream::cancel(){
		stream_.cancel();
	}

	BinaryStream& operator>>(BinaryStream& stream, bool& b) {
		uint8_t v;
		stream >> v;
		b = (v != 0);
		return stream;
	}

	BinaryStream& operator>>(BinaryStream& stream, uint8_t& v) {
		v = 0;
		stream.read(&v, 1);
		return stream;
	}

	BinaryStream& operator>>(BinaryStream& stream, int8_t& v) {
		uint8_t u;
		stream >> u;
		v = u;
		return stream;
	}

	BinaryStream& operator>>(BinaryStream& stream, uint16_t& v) {
		uint8_t s[2] = {0, 0};
		stream.read(s, 2);
		v = (s[0] << 8) | s[1];
		return stream;
	}

	BinaryStream& operator>>(BinaryStream& stream, int16_t& v) {
		uint16_t u;
		stream >> u;
		v = u;
		return stream;
	}

	BinaryStream& operator>>(BinaryStream& stream, uint32_t& v) {
		uint8_t s[4] = {0, 0, 0, 0};
		stream.read(s, 4);
		v = (uint32_t(s[0]) << 24) | (uint32_t(s[1]) << 16) | (uint32_t(s[2]) << 8) | uint32_t(s[3]);
		return stream;
	}

	BinaryStream& operator>>(BinaryStream& stream, int32_t& v) {
		uint32_t u;
		stream >> u;
		v = u;
		return stream;
	}

	BinaryStream& operator>>(BinaryStream& stream, uint64_t& v) {
		uint8_t s[8] = {0, 0, 0, 0, 0, 0, 0, 0};
		stream.read(s, 8);
		v = (uint64_t(s[0]) << 56) | (uint64_t(s[1]) << 48) | (uint64_t(s[2]) << 40) | (uint64_t(s[3]) << 32)
			| (uint64_t(s[4]) << 24) | (uint64_t(s[5]) << 16) | (uint64_t(s[6]) << 8) | uint64_t(s[7]);
		return stream;
	}

	BinaryStream& operator>>(BinaryStream& stream, int64_t& v) {
		uint64_t u;
		stream >> u;
		v = u;
		return stream;
	}

	BinaryStream& operator>>(BinaryStream& stream, std::string& string) {
		uint8_t data[string.length()];
		stream.read(data, string.length());
		string.assign((const char *) data, string.length());
		return stream;
	}

	BinaryStream& operator>>(BinaryStream& stream, Buffer& buffer){
		BufferBuilder builder(buffer);
		BinaryStream writeStream(builder);

		uint64_t bufferSize;
		stream >> bufferSize;

		uint8_t data[1024];
		std::size_t size;
		while((size = stream.tryRead(data, std::min<uint32_t>(1024, bufferSize))) != 0){
			writeStream.write(data, size);
			bufferSize -= size;
		}

		return stream;
	}

	BinaryStream& operator<<(BinaryStream& stream, bool b) {
		return stream << uint8_t(b ? 1 : 0);
	}

	BinaryStream& operator<<(BinaryStream& stream, uint8_t v) {
		stream.write(&v, 1);
		return stream;
	}

	BinaryStream& operator<<(BinaryStream& stream, int8_t v) {
		uint8_t u = v;
		stream << u;
		return stream;
	}

	BinaryStream& operator<<(BinaryStream& stream, uint16_t v) {
		uint8_t s[2];
		s[0] = (v >> 8) & 0xFF;
		s[1] = v & 0xFF;
		stream.write(s, 2);
		return stream;
	}

	BinaryStream& operator<<(BinaryStream& stream, int16_t v) {
		uint16_t u = v;
		stream << u;
		return stream;
	}

	BinaryStream& operator<<(BinaryStream& stream, uint32_t v) {
		uint8_t s[4];
		s[0] = (v >> 24) & 0xFF;
		s[1] = (v >> 16) & 0xFF;
		s[2] = (v >> 8) & 0xFF;
		s[3] = v & 0xFF;
		stream.write(s, 4);
		return stream;
	}

	BinaryStream& operator<<(BinaryStream& stream, int32_t v) {
		uint32_t u = v;
		stream << u;
		return stream;
	}

	BinaryStream& operator<<(BinaryStream& stream, uint64_t v) {
		uint8_t s[8];
		s[0] = (v >> 56) & 0xFF;
		s[1] = (v >> 48) & 0xFF;
		s[2] = (v >> 40) & 0xFF;
		s[3] = (v >> 32) & 0xFF;
		s[4] = (v >> 24) & 0xFF;
		s[5] = (v >> 16) & 0xFF;
		s[6] = (v >> 8) & 0xFF;
		s[7] = v & 0xFF;
		stream.write(s, 8);
		return stream;
	}

	BinaryStream& operator<<(BinaryStream& stream, int64_t v) {
		uint64_t u = v;
		stream << u;
		return stream;
	}

	BinaryStream& operator<<(BinaryStream& stream, const char * string){
		std::string s(string);
		return stream << s;
	}

	BinaryStream& operator<<(BinaryStream& stream, const std::string& string) {
		const uint8_t * data = (const uint8_t *) string.c_str();
		stream.write(data, string.length());
		return stream;
	}

	BinaryStream& operator<<(BinaryStream& stream, const Buffer& buffer){
		BufferIterator iterator(buffer);
		BinaryStream readStream(iterator);

		uint64_t bufferSize = buffer.size();
		stream << bufferSize;

		uint8_t data[1024];
		std::size_t size = 1;
		while((size = readStream.tryRead(data, 1024)) != 0){
			stream.write(data, size);
		}

		return stream;
	}

}
