#ifndef OPENP2P_OFTORRENT_BLOCKSTREAM_HPP
#define OPENP2P_OFTORRENT_BLOCKSTREAM_HPP

#include <OpenP2P/Stream.hpp>
#include <OpenP2P/OFTorrent/StreamGenerator.hpp>

namespace OpenP2P{

	namespace OFTorrent{

		enum BlockSize{
			BLOCKSIZE_512KB = 512 * 1024
		};

		class BlockStream: public OutputStream{
			public:
				BlockStream(OutputStreamGenerator&, BlockSize);

				std::size_t waitForSpace(Timeout timeout);

				bool write(const uint8_t * data, std::size_t dataSize, Timeout timeout);

			private:
				OutputStreamGenerator& generator_;
				OutputStream * stream_;
				std::size_t blockPos_, blockSize_;

		};

	}

}

#endif
