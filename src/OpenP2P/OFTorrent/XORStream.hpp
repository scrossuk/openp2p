#ifndef OPENP2P_OFTORRENT_XORSTREAM_HPP
#define OPENP2P_OFTORRENT_XORSTREAM_HPP

#include <stdint.h>

#include <OpenP2P/BufferedStream.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P {

	namespace OFTorrent {
	
		/**
		 * A stream that reads from two streams, and xors them together.
		 */
		class XORStream: public IStream {
			public:
				XORStream(IStream& source0, IStream& source1);
				
				bool isValid() const;
				
				size_t read(uint8_t* data, size_t size);
				
			private:
				BufferedStream source0_;
				BufferedStream source1_;
				
		};
		
	}
	
}

#endif
