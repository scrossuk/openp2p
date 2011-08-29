#ifndef OPENP2P_STRINGSTREAM_HPP
#define OPENP2P_STRINGSTREAM_HPP

#include <stdint.h>
#include <cstddef>
#include <string>

#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	class StringIStream: public IStream{
		public:
			StringIStream(const std::string&);
			
			EventHandle readEvent();

			std::size_t readSome(uint8_t * data, std::size_t dataSize);

		private:
			const std::string& string_;
			std::size_t pos_;

	};

}

#endif
