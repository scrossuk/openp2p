#ifndef OPENP2P_STRINGSTREAM_HPP
#define OPENP2P_STRINGSTREAM_HPP

#include <stdint.h>
#include <cstddef>
#include <string>

#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	class StringIStream: public InputStream{
		public:
			StringIStream(const std::string&);
			
			std::size_t waitForData(Timeout);

			bool read(uint8_t * data, std::size_t size, Timeout);

		private:
			const std::string& string_;
			std::size_t pos_;

	};

}

#endif
