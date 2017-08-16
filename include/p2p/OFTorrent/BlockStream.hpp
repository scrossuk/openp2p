#ifndef P2P_OFTORRENT_BLOCKSTREAM_HPP
#define P2P_OFTORRENT_BLOCKSTREAM_HPP

#include <p2p/OFTorrent/OFTorrent_api.hpp>
#include <p2p/Transport/Stream.hpp>
#include <p2p/OFTorrent/StreamGenerator.hpp>

namespace p2p {

	namespace OFTorrent {
	
		enum BlockSize {
			BLOCKSIZE_512KB = 512 * 1024
		};
		
		class OFTORRENT_OPENP2P_API BlockStream: public OStream {
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
