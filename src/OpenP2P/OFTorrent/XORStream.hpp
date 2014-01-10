#ifndef OPENP2P_OFTORRENT_XORSTREAM_HPP
#define OPENP2P_OFTORRENT_XORSTREAM_HPP

#include <OpenP2P/Stream.hpp>

namespace OpenP2P {

	namespace OFTorrent {
	
		/**
		 * A stream that reads from two streams, and xors them together.
		 */
		class XORStream: public InputStream {
			public:
				XORStream(InputStream& source0, InputStream& source1);
				
				std::size_t waitForData(Timeout timeout);
				
				bool read(uint8_t* data, std::size_t size, Timeout timeout);
				
			private:
				InputStream& source0_;
				InputStream& source1_;
				
		};
		
	}
	
}

#endif
