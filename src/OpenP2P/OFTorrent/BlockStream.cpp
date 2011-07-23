#include <algorithm>
#include <OpenP2P/Stream.hpp>
#include <OpenP2P/OFTorrent/BlockStream.hpp>
#include <OpenP2P/OFTorrent/StreamGenerator.hpp>

namespace OpenP2P{

	namespace OFTorrent{

		BlockOStream::BlockOStream(OStreamGenerator& generator, BlockSize size)
			: generator_(generator), blockPos_(0), blockSize_(size){
			stream_ = &(generator_.getOStream());
		}

		Future<std::size_t> BlockOStream::writeSome(const Block& block){
			std::size_t writeSize = (stream_->writeSome(block.substr(0, blockSize_ - blockPos_))).get();
			blockPos_ += writeSize;
			if(blockPos_ == blockSize_){
				stream_ = &(generator_.getOStream());
				blockPos_ = 0;
			}
			return writeSize;
		}

	}

}

