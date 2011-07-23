#include <cstddef>

#include <OpenP2P/Block.hpp>
#include <OpenP2P/BufferedStream.hpp>
#include <OpenP2P/Future.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	BufferedStream::BufferedStream(IStream& stream)
		: stream_(stream), activeRead_(false){ }
		
	Future<Block> BufferedStream::readSome(){
		if(future_.isReady()){
			// Just finished a read.
			if(activeRead_){
				block_ = future_.get();
				activeRead_ = false;
			}
			
			if(block_.size() == 0){
				future_ = stream_.readSome();
				activeRead_ = true;
				return future_;
			}else{
				return block_;
			}
		}else{
			// Previous read hasn't completed yet.
			return future_;
		}
	}

	void BufferedStream::consume(std::size_t consumeSize){
		if(future_.isReady()){
			// Just finished a read.
			if(activeRead_){
				block_ = future_.get();
				activeRead_ = false;
			}
			
			block_ = block_.substr(consumeSize);
		}
	}

}

