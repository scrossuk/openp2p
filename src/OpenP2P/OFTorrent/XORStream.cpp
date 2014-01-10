#include <algorithm>
#include <cstddef>

#include <OpenP2P/EventHandle.hpp>
#include <OpenP2P/Stream.hpp>
#include <OpenP2P/Timeout.hpp>
#include <OpenP2P/TimeoutSequence.hpp>

#include <OpenP2P/OFTorrent/XORStream.hpp>

namespace OpenP2P {

	namespace OFTorrent {
	
		static const std::size_t BufferSize = 256;
		
		XORStream::XORStream(InputStream& source0, InputStream& source1) : source0_(source0), source1_(source1) { }
		
		std::size_t XORStream::waitForData(Timeout timeout) {
			TimeoutSequence sequence(timeout);
			const std::size_t source0DataSize = source0_.waitForData(sequence.getTimeout());
			const std::size_t source1DataSize = source1_.waitForData(sequence.getTimeout());
			
			return std::min(BufferSize, std::min(source0DataSize, source1DataSize));
		}
		
		bool XORStream::read(uint8_t* data, std::size_t size, Timeout timeout) {
			if (size > BufferSize) {
				return false;
			}
			
			uint8_t buffer[BufferSize];
			
			TimeoutSequence sequence(timeout);
			
			if (!source0_.read(data, size, sequence.getTimeout())) {
				return false;
			}
			
			if (!source1_.read(buffer, size, sequence.getTimeout())) {
				return false;
			}
			
			for (std::size_t pos = 0; pos < size; pos++) {
				data[pos] ^= buffer[pos];
			}
			
			return true;
		}
		
	}
	
}

