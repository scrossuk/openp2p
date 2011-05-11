#ifndef OPENP2P_CACHESTREAM_HPP
#define OPENP2P_CACHESTREAM_HPP

#include <boost/scoped_array.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	const std::size_t DefaultCacheSize = 4096;

	class CacheStream{
		public:
			CacheStream(IStream&, std::size_t cacheSize = DefaultCacheSize);

			inline std::size_t readSome(){
				return readSome(cacheSize_);
			}

			std::size_t readSome(std::size_t);

			inline std::size_t size() const{
				return writePos_ - readPos_;
			}

			inline std::size_t cacheSize() const{
				return cacheSize_;
			}

			inline const uint8_t * get() const{
				return data_.get() + readPos_;
			}

			inline uint8_t operator[](std::size_t index) const{
				return data_[readPos_ + index];
			}

			void consume(std::size_t);

			inline void cancel(){
				stream_.cancel();
			}

		private:
			IStream& stream_;
			boost::scoped_array<uint8_t> data_;
			std::size_t cacheSize_, readPos_, writePos_;

	};

}

#endif
