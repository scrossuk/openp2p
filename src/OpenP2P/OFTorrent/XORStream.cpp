#include <OpenP2P/BufferedStream.hpp>
#include <OpenP2P/EventHandle.hpp>
#include <OpenP2P/Stream.hpp>

#include <OpenP2P/OFTorrent/XORStream.hpp>

namespace OpenP2P{

	namespace OFTorrent{

		XORIStream::XORIStream(IStream& source0, IStream& source1) : bufferedStream_(source0), stream_(source1){ }
		
		EventHandle XORIStream::readEvent(){
			return EventHandle::And(bufferedStream_.readEvent(), stream_.readEvent());
		}

		std::size_t XORIStream::readSome(uint8_t * data, std::size_t dataSize){
			const std::size_t bufferedReadSize = bufferedStream_.read(dataSize);
			const std::size_t readSize = stream_.readSome(data, bufferedReadSize);
			
			for(std::size_t pos = 0; pos < readSize; pos++){
				data[pos] ^= bufferedStream_[pos];
			}
			
			bufferedStream_.consume(readSize);
			
			return readSize;
		}

	}

}

