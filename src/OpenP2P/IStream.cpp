#include <stdint.h>
#include <cstddef>
#include <string>
#include <OpenP2P/IStream.hpp>
#include <OpenP2P/WaitHandler.hpp>

namespace OpenP2P {

	bool IStream::tryRead(uint8_t * data, std::size_t length, WaitHandler handler) {
		std::size_t p = 0;
		while (p < length) {
			std::size_t readSize = readSome(data + p, length - p, handler);
			if (readSize == 0) {
				return false;
			}
			else {
				p += readSize;
			}
		}
		return true;
	}

	void IStream::read(uint8_t * data, std::size_t length, WaitHandler handler) {
		streamIsValid_ = tryRead(data, length, handler);
	}

	IStream& operator>>(IStream& stream, bool& b) {
		uint8_t v;
		stream >> v;
		b = (v != 0);
		return stream;
	}

	IStream& operator>>(IStream& stream, uint8_t& v) {
		v = 0;
		stream.read(&v, 1);
		return stream;
	}

	IStream& operator>>(IStream& stream, int8_t& v) {
		uint8_t u;
		stream >> u;
		v = u;
		return stream;
	}

	IStream& operator>>(IStream& stream, uint16_t& v) {
		uint8_t s[2] = {0, 0};
		stream.read(s, 2);
		v = (s[0] << 8) | s[1];
		return stream;
	}

	IStream& operator>>(IStream& stream, int16_t& v) {
		uint16_t u;
		stream >> u;
		v = u;
		return stream;
	}

	IStream& operator>>(IStream& stream, uint32_t& v) {
		uint8_t s[4] = {0, 0, 0, 0};
		stream.read(s, 4);
		v = (uint32_t(s[0]) << 24) | (uint32_t(s[1]) << 16) | (uint32_t(s[2]) << 8) | uint32_t(s[3]);
		return stream;
	}

	IStream& operator>>(IStream& stream, int32_t& v) {
		uint32_t u;
		stream >> u;
		v = u;
		return stream;
	}

	IStream& operator>>(IStream& stream, uint64_t& v) {
		uint8_t s[8] = {0, 0, 0, 0, 0, 0, 0, 0};
		stream.read(s, 8);
		v = (uint64_t(s[0]) << 56) | (uint64_t(s[1]) << 48) | (uint64_t(s[2]) << 40) | (uint64_t(s[3]) << 32)
			| (uint64_t(s[4]) << 24) | (uint64_t(s[5]) << 16) | (uint64_t(s[6]) << 8) | uint64_t(s[7]);
		return stream;
	}

	IStream& operator>>(IStream& stream, int64_t& v) {
		uint64_t u;
		stream >> u;
		v = u;
		return stream;
	}

	IStream& operator>>(IStream& stream, std::string& string) {
		uint8_t data[string.length()];
		stream.read(data, string.length());
		string.assign((const char *) data, string.length());
		return stream;
	}

}
