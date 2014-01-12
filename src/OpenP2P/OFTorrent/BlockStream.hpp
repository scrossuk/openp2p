#ifndef OPENP2P_OFTORRENT_BLOCKSTREAM_HPP
#define OPENP2P_OFTORRENT_BLOCKSTREAM_HPP

#include <OpenP2P/Stream.hpp>
#include <OpenP2P/OFTorrent/StreamGenerator.hpp>

namespace OpenP2P {

	namespace OFTorrent {
	
		enum BlockSize {
			BLOCKSIZE_512KB = 512 * 1024
		};
		
		class BlockStream: public OStream {
			public:
				BlockStream(OutputStreamGenerator&, BlockSize);
				
				bool isValid() const;
				
				Event::Source eventSource() const;
				
				size_t write(const uint8_t* data, size_t dataSize);
				
			private:
				OutputStreamGenerator& generator_;
				OStream* stream_;
				size_t blockPos_, blockSize_;
				
		};
		
	}
	
}

#endif
