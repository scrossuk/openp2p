#include <stdint.h>
#include <cstddef>
#include <string>
#include <OpenP2P/OStream.hpp>
#include <OpenP2P/WaitHandler.hpp>

namespace OpenP2P {

	bool OStream::tryWrite(const uint8_t * data, std::size_t length, WaitHandler handler) {
		std::size_t p = 0;
		while (p < length) {
			std::size_t writeSize = writeSome(data + p, length - p, handler);
			if (writeSize == 0) {
				return false;
			}
			else {
				p += writeSize;
			}
		}
		return true;
	}

	void OStream::write(const uint8_t * data, std::size_t length, WaitHandler handler) {
		streamIsValid_ = tryWrite(data, length, handler);
	}

	OStream& operator<<(OStream& stream, bool b) {
		return stream << uint8_t(b ? 1 : 0);
	}

	OStream& operator<<(OStream& stream, uint8_t v) {
		stream.write(&v, 1);
		return stream;
	}

	OStream& operator<<(OStream& stream, int8_t v) {
		uint8_t u = v;
		stream << u;
		return stream;
	}

	OStream& operator<<(OStream& stream, uint16_t v) {
		uint8_t s[2];
		s[0] = (v >> 8) & 0xFF;
		s[1] = v & 0xFF;
		stream.write(s, 2);
		return stream;
	}

	OStream& operator<<(OStream& stream, int16_t v) {
		uint16_t u = v;
		stream << u;
		return stream;
	}

	OStream& operator<<(OStream& stream, uint32_t v) {
		uint8_t s[4];
		s[0] = (v >> 24) & 0xFF;
		s[1] = (v >> 16) & 0xFF;
		s[2] = (v >> 8) & 0xFF;
		s[3] = v & 0xFF;
		stream.write(s, 4);
		return stream;
	}

	OStream& operator<<(OStream& stream, int32_t v) {
		uint32_t u = v;
		stream << u;
		return stream;
	}

	OStream& operator<<(OStream& stream, uint64_t v) {
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

	OStream& operator<<(OStream& stream, int64_t v) {
		uint64_t u = v;
		stream << u;
		return stream;
	}

	OStream& operator<<(OStream& stream, const char * string){
		std::string s(string);
		return stream << s;
	}

	OStream& operator<<(OStream& stream, const std::string& string) {
		const uint8_t * data = (const uint8_t *) string.c_str();
		stream.write(data, string.length());
		return stream;
	}

}
