#ifndef P2P_OFTORRENT_XORSTREAM_HPP
#define P2P_OFTORRENT_XORSTREAM_HPP

#include <stdint.h>

#include <p2p/Stream.hpp>

#include <p2p/Buffer/BufferedStream.hpp>

namespace p2p {

	namespace OFTorrent {
	
		/**
		 * A stream that reads from two streams, and xors them together.
		 */
		class XORStream: public IStream {
			public:
				XORStream(IStream& source0, IStream& source1);
				
				bool isValid() const;
				
				Event::Source eventSource() const;
				
				size_t read(uint8_t* data, size_t size);
				
			private:
				BufferedStream source0_;
				BufferedStream source1_;
				
		};
		
	}
	
}

#endif
