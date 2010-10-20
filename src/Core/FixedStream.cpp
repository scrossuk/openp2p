#include "FixedStream.hpp"
#include "Buffer.hpp"

namespace openp2p{

	namespace Core{

		FixedStream::FixedStream(Stream * stream, unsigned int size) : mStream(stream){
			mSize = size;
		}

		FixedStream::~FixedStream(){
			delete mStream;
		}

		void FixedStream::Data(Buffer& buffer){
			mData.Add(buffer);

			while(mData.Size() >= mSize){
				Buffer data = mData.Read(mSize);
				mStream->Data(data);
			}
		}

	}

}


