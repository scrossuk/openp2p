#include <algorithm>
#include <p2p/Transport/Stream.hpp>
#include <p2p/OFTorrent/BlockStream.hpp>
#include <p2p/OFTorrent/StreamGenerator.hpp>

namespace p2p {

	namespace OFTorrent {
	
		BlockStream::BlockStream(OutputStreamGenerator& generator, BlockSize size)
			: generator_(generator), blockPos_(0), blockSize_(size) {
			stream_ = &(generator_.getNextOutputStream());
		}
		
		bool BlockStream::isValid() const {
			return stream_->isValid();
		}
		
		Event::Source BlockStream::eventSource() const {
			return stream_->eventSource();
		}
		
		size_t BlockStream::write(const uint8_t* data, size_t size) {
			const size_t maxWriteSize = std::min(size, blockSize_ - blockPos_);
			
			const size_t writeSize = stream_->write(data, maxWriteSize);
			
			blockPos_ += writeSize;
			if (blockPos_ == blockSize_) {
				stream_ = &(generator_.getNextOutputStream());
				blockPos_ = 0;
			}
			
			return writeSize;
		}
		
	}
	
}

