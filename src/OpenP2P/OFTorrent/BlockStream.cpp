#include <algorithm>
#include <OpenP2P/Stream.hpp>
#include <OpenP2P/OFTorrent/BlockStream.hpp>
#include <OpenP2P/OFTorrent/StreamGenerator.hpp>

namespace OpenP2P{

	namespace OFTorrent{

		BlockStream::BlockStream(OutputStreamGenerator& generator, BlockSize size)
			: generator_(generator), blockPos_(0), blockSize_(size){
			stream_ = &(generator_.getNextOutputStream());
		}
		
		std::size_t BlockStream::waitForSpace(Timeout timeout){
			return stream_->waitForSpace(timeout);
		}

		bool BlockStream::write(const uint8_t * data, std::size_t size, Timeout timeout){
			if(!stream_->write(data, size, timeout)){
				return false;
			}
			
			blockPos_ += size;
			if(blockPos_ == blockSize_){
				stream_ = &(generator_.getNextOutputStream());
				blockPos_ = 0;
			}
			
			return true;
		}

	}

}

