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

		std::size_t BlockOStream::writeSome(const uint8_t * data, std::size_t requestedSize){
			std::size_t maxWriteSize = std::min(requestedSize, blockSize_ - blockPos_);
			std::size_t writeSize = stream_->writeSome(data, maxWriteSize);
			blockPos_ += writeSize;
			if(blockPos_ == blockSize_){
				stream_ = &(generator_.getOStream());
				blockPos_ = 0;
			}
			return writeSize;
		}

		void BlockOStream::cancel(){
			stream_->cancel();
		}

	}

}

