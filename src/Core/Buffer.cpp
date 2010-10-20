#include <string>
#include "Types.hpp"
#include "Buffer.hpp"
#include "SharedPtr.hpp"
#include "SharedCount.hpp"

namespace openp2p{

	namespace Core{

		class BufferNode: public BufferTree{
			public:
				BufferNode(SharedPtr<BufferTree> left, SharedPtr<BufferTree> right) : mLeft(left), mRight(right){
					mSize = mLeft->GetSize() + mRight->GetSize();
				}

				SharedPtr<BufferTree> GetLeft(){
					return mLeft;
				}

				SharedPtr<BufferTree> GetRight(){
					return mRight;
				}

				const Uint8 * GetData() const{
					return 0;
				}

				unsigned int GetSize() const{
					return mSize;
				}

			private:
				SharedPtr<BufferTree> mLeft;
				SharedPtr<BufferTree> mRight;
				unsigned int mSize;

		};

		class BufferBlock: public SharedCount{
			public:
				BufferBlock(){
					mSize = 0;
					mData = 0;
				}

				BufferBlock(Uint8 v){
					mSize = 1;
					mData = new Uint8[mSize];
					mData[0] = v;
				}

				BufferBlock(Uint16 v){
					mSize = 2;
					mData = new Uint8[mSize];
					mData[0] = (v >> 8) & 0xFF;
					mData[1] = v & 0xFF;
				}

				BufferBlock(Uint32 v){
					mSize = 4;
					mData = new Uint8[mSize];
					mData[0] = (v >> 24) & 0xFF;
					mData[1] = (v >> 16) & 0xFF;
					mData[2] = (v >> 8) & 0xFF;
					mData[3] = v & 0xFF;
				}

				BufferBlock(const Uint8 * data, unsigned int dataSize){
					mSize = dataSize;
					mData = new Uint8[mSize];

					for(unsigned int i = 0; i < mSize; ++i){
						mData[i] = data[i];
					}
				}

				BufferBlock(const std::string& str){
					unsigned int size = str.size();

					mSize = size + 2;
					mData = new Uint8[mSize];

					mData[0] = (size >> 8) & 0xFF;
					mData[1] = size & 0xFF;
					Uint8 * data = mData + 2;

					for(unsigned int i = 0; i < mSize; ++i){
						data[i] = str[i];
					}
				}

				const Uint8 * GetData() const{
					return mData;
				}

				unsigned int GetSize() const{
					return mSize;
				}

			protected:
				~BufferBlock(){
					delete[] mData;
				}

			private:
				Uint8 * mData;
				unsigned int mSize;

		};

		class BufferLeaf: public BufferTree{
			public:
				BufferLeaf(SharedPtr<BufferBlock> block) : mBlock(block){
					mStart = 0;
					mEnd = mBlock->GetSize();
				}

				BufferLeaf(SharedPtr<BufferBlock> block, unsigned int start, unsigned int end) : mBlock(block){
					mStart = start;
					mEnd = end;
				}

				SharedPtr<BufferTree> GetLeft(){
					return this;
				}

				SharedPtr<BufferTree> GetRight(){
					return this;
				}

				const Uint8 * GetData() const{
					return mBlock->GetData() + mStart;
				}

				unsigned int GetSize() const{
					return mEnd - mStart;
				}

			private:
				SharedPtr<BufferBlock> mBlock;
				unsigned int mStart, mEnd;

		};

		Buffer::Buffer()
			: mTree(new BufferLeaf(new BufferBlock())){ }

		Buffer::Buffer(Uint8 v)
			: mTree(new BufferLeaf(new BufferBlock(v))){ }

		Buffer::Buffer(Uint16 v)
			: mTree(new BufferLeaf(new BufferBlock(v))){ }

		Buffer::Buffer(Uint32 v)
			: mTree(new BufferLeaf(new BufferBlock(v))){ }

		Buffer::Buffer(const Uint8 * data, unsigned int dataSize)
			: mTree(new BufferLeaf(new BufferBlock(data, dataSize))){ }

		Buffer::Buffer(const std::string& str)
			: mTree(new BufferLeaf(new BufferBlock(str))){ }

		Buffer::Buffer(const Buffer& buffer)
			: mTree(buffer.mTree){ }

		Buffer::Buffer(const Buffer& a, const Buffer& b)
			: mTree(new BufferNode(a.mTree, b.mTree)){ }

		Buffer Buffer::operator+(const Buffer& buffer) const{
			return Buffer(*this, buffer);
		}

		SharedPtr<BufferTree> Buffer::GetTree() const{
			return mTree;
		}

		unsigned int Buffer::Size() const{
			return mTree->GetSize();
		}

	}

}
