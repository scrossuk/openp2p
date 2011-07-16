#include <stdint.h>
#include <cstddef>
#include <OpenP2P/Stream.hpp>
#include <OpenP2P/OFTorrent/XORStream.hpp>

namespace OpenP2P{

	namespace OFTorrent{

		XORIStream::XORIStream(IStream& source1, IStream& source2) : buffered1_(source1), buffered2_(source2){ }

		Future<std::size_t> XORIStream::readSome(uint8_t * data, std::size_t requestedSize){
			Future<std::size_t> read1 = buffered1_.readSome(requestedSize);
			Future<std::size_t> read2 = buffered2_.readSome(requestedSize);
		
			return read1.lateJoin(read2).compose<std::size_t>(boost::bind(&XORIStream::onRead, this, data, _1));
		}

		std::size_t XORIStream::onRead(uint8_t * data, std::pair<std::size_t, std::size_t> readSizes){
			std::size_t readSize = std::min(readSizes.first, readSizes.second);
			
			const uint8_t * firstPtr = buffered1_.get();
			const uint8_t * secondPtr = buffered2_.get();
			
			for(std::size_t i = 0; i < readSize; i++){
				data[i] = firstPtr[i] ^ secondPtr[i];
			}
			
			buffered1_.consume(readSize);
			buffered2_.consume(readSize);
			return readSize;
		}

	}

}

