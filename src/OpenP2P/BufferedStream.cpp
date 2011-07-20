#include <stdint.h>
#include <algorithm>
#include <cstddef>

#include <boost/bind.hpp>

#include <OpenP2P/BufferedStream.hpp>
#include <OpenP2P/Future.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	namespace{
	
		std::size_t add(std::size_t v0, std::size_t v1){
			return v0 + v1;
		}
	
	}

	BufferedStream::BufferedStream(IStream& stream, std::size_t bufferSize)
		: stream_(stream), data_(new uint8_t[bufferSize]), bufferSize_(bufferSize),
		readPos_(0), size_(0), future_(0), activeRead_(false){ }
		
	Future<std::size_t> BufferedStream::readSome(std::size_t requestedSize){
		if(activeRead_){
			if(future_.isReady()){
				// Update total size.
				size_ = future_.get();
				activeRead_ = false;
			}else{
				// Still reading.
				return future_.compose<std::size_t>(boost::bind(std::min<std::size_t>, requestedSize, _1));
			}
		}
		
		std::size_t availableSpace = bufferSize_ - size_ - readPos_;
		
		if(requestedSize <= size_){
			// Enough data has already been read.
			return requestedSize;
		}else{
			// Request size is larger than current size - try to read more data.
			if(availableSpace == 0){
				// No space to do a read - return what has been read before.
				return size_;
			}else{
				// Perform a read to get more data.
				
				// Never read more than requested.
				std::size_t maxReadSize = std::min(availableSpace, requestedSize - size_);
			
				activeRead_ = true;
			
				// Return a future that resolves to the full size of the buffered stream, while only reading the necessary amount.
				return future_ = stream_.readSome(data_.get() + readPos_ + size_, maxReadSize).compose<std::size_t>(boost::bind(add, size_, _1));
			}
		}
	}

	void BufferedStream::consume(std::size_t requestedSize){
		// Update the total size.
		if(future_.isReady() && activeRead_){
			size_ = future_.get();
			activeRead_ = false;
		}
		
		std::size_t readSize = std::min(requestedSize, size_);
		readPos_ += readSize;
		size_ -= readSize;
		
		if(readPos_ == bufferSize_){
			// Reached the end of the buffer - go to the beginning.
			readPos_ = 0;
		}
	}

}

