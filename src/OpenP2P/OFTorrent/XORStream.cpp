#include <stdint.h>

#include <algorithm>

#include <OpenP2P/Stream.hpp>

#include <OpenP2P/Buffer/BufferedStream.hpp>

#include <OpenP2P/OFTorrent/XORStream.hpp>

namespace OpenP2P {

	namespace OFTorrent {
	
		XORStream::XORStream(IStream& source0, IStream& source1) : source0_(source0), source1_(source1) { }
		
		bool XORStream::isValid() const {
			return source0_.isValid() && source1_.isValid();
		}
		
		Event::Source XORStream::eventSource() const {
			// TODO: get events from both source streams!
			return source0_.eventSource();
		}
		
		size_t XORStream::read(uint8_t* data, size_t size) {
			const size_t source0Size = source0_.readMax();
			const size_t source1Size = source1_.readMax();
			
			const size_t readSize = std::min(std::min(source0Size, source1Size), size);
			
			for (size_t i = 0; i < readSize; i++) {
				data[i] = source0_[i] ^ source1_[i];
			}
			
			return readSize;
		}
		
	}
	
}

