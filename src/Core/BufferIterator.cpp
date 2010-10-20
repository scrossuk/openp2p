#include <stack>
#include "Types.hpp"
#include "Buffer.hpp"
#include "BufferIterator.hpp"
#include "SharedPtr.hpp"

namespace openp2p{

	namespace Core{

		BufferIterator::BufferIterator(){
			mBuffer = new Buffer();
			Seek(0);
		}

		BufferIterator::BufferIterator(Buffer buffer, unsigned int offset){
			mBuffer = new Buffer(buffer);
			Seek(offset);
		}

		BufferIterator::~BufferIterator(){
			delete mBuffer;
		}

		unsigned int BufferIterator::GetPosition(){
			return mPosition;
		}
		
		unsigned int BufferIterator::Remaining(){
			return mBuffer->Size() - mPosition;
		}

		void BufferIterator::SetBuffer(Buffer buffer, unsigned int offset){
			if(offset > buffer.Size()){
				throw BufferOutOfBoundsException();
			}

			delete mBuffer;
			mBuffer = new Buffer(buffer);
			Seek(offset);
		}

		void BufferIterator::Seek(unsigned int offset){
			if(offset > mBuffer->Size()){
				throw BufferOutOfBoundsException();
			}

			mPosition = offset;

			for(unsigned int i = 0; i < mStack.size(); ++i){
				mStack.pop();
			}

			SharedPtr<BufferTree> node = mBuffer->GetTree();
			SharedPtr<BufferTree> left = node->GetLeft();

			while(node != left){
				unsigned int midPoint = left->GetSize();
				mStack.push(node);
				if(offset < midPoint){
					node = left;
				}else{
					node = node->GetRight();
					offset -= midPoint;
				}
				left = node->GetLeft();
			}

			mStack.push(node);

			mNodePos = offset;
		}

		Buffer BufferIterator::GetBuffer() const{
			return *mBuffer;
		}

		void BufferIterator::Read(Uint8 * data, unsigned int dataSize){
			unsigned int startPosition = mPosition;
			while(true){
				unsigned int maxReadSize = mStack.top()->GetSize() - mNodePos;
				const Uint8 * readData = mStack.top()->GetData() + mNodePos;

				unsigned int readSize = (dataSize <= maxReadSize) ? dataSize : maxReadSize;
				dataSize -= readSize;

				for(unsigned int i = 0; i < readSize; ++i){
					data[i] = readData[i];
				}

				data += readSize;
				mNodePos += readSize;
				mPosition += readSize;

				if(dataSize == 0){
					return;
				}

				try{
					MoveToSuccessor();
				}catch(BufferOutOfBoundsException){
					Seek(startPosition);
					throw BufferOutOfBoundsException();
				}
			}
		}

		Buffer BufferIterator::Read(unsigned int dataSize){
			Uint8 data[dataSize];
			Read(data, dataSize);
			return Buffer(data, dataSize);
		}

		void BufferIterator::MoveToSuccessor(){
			if(mStack.size() == 1){
				throw BufferOutOfBoundsException();
			}

			SharedPtr<BufferTree> child = mStack.top();

			mStack.pop();

			SharedPtr<BufferTree> node = mStack.top();

			while(child == node->GetRight()){
				if(mStack.empty()){
					throw BufferOutOfBoundsException();
				}

				child = node;
				mStack.pop();
				node = mStack.top();
			}

			node = node->GetRight();
			child = node->GetLeft();

			while(node != child){
				mStack.push(node);
				node = child;
				child = node->GetLeft();
			}

			mNodePos = 0;

			mStack.push(node);
		}

	}

}
