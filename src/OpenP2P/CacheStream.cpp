#include <algorithm>
#include <OpenP2P/CacheStream.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	CacheStream::CacheStream(IStream& stream, std::size_t cacheSize)
		: stream_(stream), data_(new uint8_t[cacheSize]), cacheSize_(cacheSize), readPos_(0), writePos_(0){ }

	std::size_t CacheStream::readSome(std::size_t requestedSize){
		if(requestedSize <= size()) return requestedSize;
		std::size_t availableSpace = cacheSize_ - writePos_;
		std::size_t maxReadSize = std::min(availableSpace, requestedSize - size());
		writePos_ += stream_.readSome(data_.get() + writePos_, maxReadSize);
		return std::min(requestedSize, size());
	}

	void CacheStream::consume(std::size_t requestedSize){
		readPos_ += std::min(requestedSize, size());
		if(readPos_ == cacheSize_) readPos_ = writePos_ = 0;
	}

}

