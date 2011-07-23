#ifndef OPENP2P_OFTORRENT_BLOCKSTREAM_HPP
#define OPENP2P_OFTORRENT_BLOCKSTREAM_HPP

#include <OpenP2P/Stream.hpp>
#include <OpenP2P/OFTorrent/StreamGenerator.hpp>

namespace OpenP2P{

	namespace OFTorrent{

		enum BlockSize{
			BLOCKSIZE_512KB = 512 * 1024
		};

		class BlockOStream: public OStream{
			public:
				BlockOStream(OStreamGenerator&, BlockSize);

				Future<std::size_t> writeSome(const Block& block);

			private:
				OStreamGenerator& generator_;
				OStream * stream_;
				std::size_t blockPos_, blockSize_;

		};

	}

}

#endif
