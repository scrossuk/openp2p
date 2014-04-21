#ifndef P2P_STRINGSTREAM_HPP
#define P2P_STRINGSTREAM_HPP

#include <stdint.h>

#include <string>

#include <p2p/Stream.hpp>

namespace p2p {

	class StringIStream: public IStream {
		public:
			StringIStream(const std::string&);
			
			bool isValid() const;
			
			size_t read(uint8_t* data, size_t size);
			
		private:
			const std::string& string_;
			size_t position_;
			
	};
	
}

#endif
