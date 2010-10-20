#include <queue>
#include "Types.hpp"
#include "Buffer.hpp"
#include "BufferIterator.hpp"
#include "BufferQueue.hpp"

namespace openp2p{

	namespace Core{

		BufferQueue::BufferQueue(){
			mSize = mCurrentRemaining = 0;
		}

		void BufferQueue::Add(Buffer buffer){
			if(mSize == 0){
				mCurrentRemaining = buffer.Size();
				mIterator.SetBuffer(buffer);
			}
			mQueue.push(buffer);
			mSize += buffer.Size();
		}

		unsigned int BufferQueue::Size(){
			return mSize;
		}

		void BufferQueue::Read(Uint8 * data, unsigned int dataSize){
			while(true){
				unsigned int readSize = (dataSize <= mCurrentRemaining) ? dataSize : mCurrentRemaining;

				mIterator.Read(data, readSize);
				dataSize -= readSize;
				data += readSize;
				mSize -= readSize;
				mCurrentRemaining -= readSize;

				if(dataSize == 0){
					return;
				}

				if(mCurrentRemaining == 0){
					if(mQueue.size() == 1){
						throw BufferOutOfBoundsException();
					}else{
						mQueue.pop();
						mCurrentRemaining = mQueue.front().Size();
						mIterator.SetBuffer(mQueue.front());
					}
				}
			}
		}

		Buffer BufferQueue::Read(unsigned int dataSize){
			Uint8 data[dataSize];
			Read(data, dataSize);
			return Buffer(data, dataSize);
		}
		
		Buffer BufferQueue::ReadAll(){
			return Read(Size());
		}

	}

}
