#include <cstddef>
#include <stdint.h>
#include <queue>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/BufferIterator.hpp>
#include <OpenP2P/BufferQueue.hpp>
#include <OpenP2P/IStream.hpp>
#include <OpenP2P/WaitHandler.hpp>

namespace OpenP2P{

	BufferQueue::BufferQueue(){ }

	void BufferQueue::add(const Buffer& buffer){
		if(buffer.size() == 0){
			return;
		}
		if(iterator_.remaining() == 0){
			iterator_.set(buffer);
		}else{
			queue_.push(buffer);
		}
	}

	std::size_t BufferQueue::readSome(uint8_t * data, std::size_t dataSize, WaitHandler handler){
		std::size_t readSize = iterator_.readSome(data, dataSize, handler);
		if(iterator_.remaining() == 0 && !queue_.empty()){
			iterator_.set(queue_.front());
			queue_.pop();
		}
		return readSize;
	}

}
