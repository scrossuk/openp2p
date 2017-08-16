#ifndef P2P_OFTORRENT_XORSTREAM_HPP
#define P2P_OFTORRENT_XORSTREAM_HPP

#include <stdint.h>

#include <p2p/OFTorrent/OFTorrent_api.hpp>
#include <p2p/Transport/Stream.hpp>
#include <p2p/Transport/BufferedStream.hpp>

namespace p2p {

	namespace OFTorrent {
	
		/**
		 * A stream that reads from two streams, and xors them together.
		 */
		class OFTORRENT_OPENP2P_API XORStream: public IStream {
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
