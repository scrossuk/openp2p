#include <stdint.h>
#include <algorithm>
#include <cstddef>
#include <OpenP2P/Stream.hpp>
#include <OpenP2P/OFTorrent/XORStream.hpp>

namespace OpenP2P{

	namespace OFTorrent{

		XORIStream::XORIStream(IStream& source1, IStream& source2) : cacheStream_(source1), source2_(source2){ }

		std::size_t XORIStream::readSome(uint8_t * data, std::size_t requestedSize){
			std::size_t cacheReadSize = cacheStream_.readSome(requestedSize);
			std::size_t readSize = source2_.readSome(data, std::min(requestedSize, cacheReadSize));
			
			for(std::size_t i = 0; i < readSize; i++){
				data[i] ^= cacheStream_[i];
			}

			cacheStream_.consume(readSize);

			return readSize;
		}

		void XORIStream::cancel(){
			cacheStream_.cancel();
			source2_.cancel();
		}

	}

}

